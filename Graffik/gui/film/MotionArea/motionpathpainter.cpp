#include "motionpathpainter.h"

#include <QDebug>

MotionPathPainter::MotionPathPainter(unsigned short c_addr, FilmParameters *c_parms, AxisOptions *c_aopt, QWidget *parent) : QWidget(parent) {
    m_path = new QPainterPath;

    m_addr = c_addr;
    m_film = c_parms;
    m_aopt = c_aopt;

    m_maxPoints = MPP_DEF_MAX_PTS;
    m_wasWidth  = 0;
    m_wasHeight = 0;
    m_wasStart  = 0;
    m_wasEnd    = 0;
    m_wasDist   = 0;
    m_wasAcc    = 0;
    m_wasDcc    = 0;
    m_wasEase   = 0;
    m_wasLength = 0;

    m_new           = false;
    m_relativeScale = false;

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(paramsChanged()));

}

MotionPathPainter::~MotionPathPainter() {
    delete m_path;
}


/** Set Display Scaling

  Choose between relative and proportional scaling.

  true = relative scaling
  false = proportional scaling

  In relative scaling, the height of the motion line indicates the current speed relative to
  the configured maximum speed of the axis.

  In proportional scaling, the height of the motion hits 95% of the total motion area height
  when the motor is moving at the top speed the movement requires.

  */

void MotionPathPainter::scaling(bool p_scale) {

    qDebug() << "MPP: Scale change: " << p_scale;
    m_relativeScale = p_scale;
        // trigger re-draw on next query
    m_new = true;
}

/** Get Display Scaling */

bool MotionPathPainter::scaling() {
    return m_relativeScale;
}

/** Set the Maximum Number of Curve Points

  This method sets the maximum number of curve points to be generated
  when the motion parameters for an axis changes.  This sets the ceiling for
  the maximum width of the rendered curve.  That is to say, the curve area
  you plot must always be less than or equal to the maximum number of curve
  points.  E.g. if max curve points is set to 2,000, the curve you plot
  can never be wider than 2,000 pixels.

  The default maximum is defined by MPP_DEF_MAX_PTS
  */

void MotionPathPainter::maxSplinePoints(unsigned long p_points) {
    if( p_points == 0 )
        return;

    m_maxPoints = p_points;
}

/** Get the Maximum Number of Curve Points */

unsigned long MotionPathPainter::maxSplinePoints() {
    return m_maxPoints;
}

/** Set a new Motion Curve

  This method generates the motion profile for an axis, given a
  the address of the axis, and a reference to a specified FilmParameters
  object.

  This method only generates the curve information (speed at points in
  time), and does NOT render a new PainterPath.  Use this method when
  the FilmParameters data changes to update the path.

  This method will take no action if the motion profile for the axis
  specified has not changed since this method was previously run.

  To ensure automatic updating with minimal impact, this method will
  connect the FilmParameters::paramsReleased() signal to the paramsChanged()
  slot.  That slot will re-call this method every time Film Parameters are
  released, ensuring that the Motion Curve data is kept up to date.  Therefore,
  it is only necessary to call this method once in the life of the MotionPathPainter
  object unless the axis address or FilmParameters object changes.

  */



void MotionPathPainter::setMotionCurve() {

    qDebug() << "MPP: setting motion curve for" << m_addr;



    OMfilmParams filmParams    = m_film->getParamsCopy();
    OMfilmAxisParams*  axParms = filmParams.axes.value(m_addr);

    m_axis = axParms;

        // do nothing more if no movement data has changed
    if ( axParms->endDist == m_wasDist &&
         axParms->endTm == m_wasEnd &&
         axParms->startTm == m_wasStart &&
         axParms->accelTm == m_wasAcc &&
         axParms->decelTm == m_wasDcc &&
         axParms->easing == m_wasEase &&
         filmParams.realLength == m_wasLength) {

        qDebug() << "MPP: Curve: No Change";
        return;
    }

        // wipe out all saved points

    m_renderPoints.clear();


    if( axParms->endDist < 1 ) {
        qDebug() << "MPP: Curve: No Distance";
        return;
    }

    unsigned long endTm = m_axis->endTm;
    unsigned long begTm = m_axis->startTm;
    unsigned long walTm = filmParams.realLength;

        // if no end time is specified, then the move ends when the film does
    if( endTm == 0 )
        endTm = walTm;

    _initSpline(axParms->endDist, endTm, axParms->accelTm, axParms->decelTm, m_maxPoints);

    for(unsigned long i = 1; i <= m_maxPoints; i++) {


          float tmPos = (float) i / (float) m_maxPoints;
          float curSpd = 0.0;

          if( axParms->easing == OM_MOT_LINEAR )
              curSpd = _linearEasing(tmPos);
          else if( axParms->easing == OM_MOT_QUAD )
              curSpd = _qEaseCalc(tmPos);
          else
              curSpd = _qInvCalc(tmPos);

          m_renderPoints.append(curSpd);
          //qDebug() << "MPP: Curve: Render" << i << curSpd;
    }


    qDebug() << "MPP: Render Amount: " << m_renderPoints.count();

    m_wasDist   = axParms->endDist;
    m_wasEnd    = axParms->endTm;
    m_wasStart  = axParms->startTm;
    m_wasAcc    = axParms->accelTm;
    m_wasDcc    = axParms->decelTm;
    m_wasEase   = axParms->easing;

    m_new = true;

}

/** Get Path Model

  Returns a QPainterPath representing the movement curve, scaled to
  the supplied QRect.

  If the QRect doesn't change its width or height, no action will be
  taken and the pre-existing QPainterPath will be returned.  If it has
  changed, a new path will be generated from existing curve data, scaled
  to the current QRect.

  */

QPainterPath* MotionPathPainter::getPath(QRect p_area) {

    qDebug() << "MPP: Get Path" << m_addr;

    m_area = p_area;

    OMfilmParams filmParams = m_film->getParamsCopy();

        // do nothing if we've already drawn this
    if( p_area.height() == m_wasHeight && p_area.width() == m_wasWidth && m_new == false && filmParams.realLength == m_wasLength)
        return m_path;

    qDebug() << "MPP: Rescaling";

    m_new = false;
    m_wasWidth = p_area.width();
    m_wasHeight = p_area.height();
    m_wasLength = filmParams.realLength;

    OMaxisOptions* aopts = m_aopt->getOptions(m_addr);

    QPoint start = p_area.bottomLeft();

    int width = p_area.width();
    int height = p_area.height();

    unsigned long ms_per_xpix = filmParams.realLength / width;

    unsigned long splines = width; // (filmParams.realLength - m_axis->startTm - m_axis->endTm) / ms_per_xpix;
    float scale = (float) m_maxPoints / (float) splines;

    int scaleMajor = int(scale);
    scale -= scaleMajor;
    float scaleErr = 0.0;

   // qDebug() << "MPP: " << ms_per_xpix << splines << width << height << endTm << m_parms->accelTm << m_parms->decelTm;

    int startPoint = start.x() + (m_axis->startTm * ms_per_xpix);

        // maxSteps in axis options is steps/second...
        // so we need seconds per (horizontal, using all points) pixel


    int spd_per_ypix;

    if( m_relativeScale == true ) {
        float displaySecs = filmParams.realLength / 1000; // seconds
        float pixelSecs = displaySecs / m_maxPoints;
        spd_per_ypix = ((float)height * 0.95) / (pixelSecs * aopts->maxSteps);
    }
    else {
        spd_per_ypix = ((float)height * 0.95) / m_splinePlanned.topSpeed;
    }

    delete m_path;
    m_path = new QPainterPath;

    m_path->moveTo(startPoint, 0);
    int curPoint = startPoint;
    int curIdx = 0;

    qDebug()  << "MPP: Scale: " << scale << splines << m_maxPoints << ms_per_xpix;

    for( unsigned long i = 0; i < splines; i++ ) {

        if( curIdx > m_renderPoints.count() - 1 ) {
            qDebug() << "MPP: IDX" << curIdx << "Greater than renderPoints max" << m_renderPoints.count();
            break;
        }

        float curSpd = m_renderPoints[curIdx];
        int hgt = spd_per_ypix * curSpd;
        m_path->lineTo(curPoint, height - hgt);
        curPoint++;
        curIdx += scaleMajor;
        scaleErr += scale;

        if( scaleErr >= 1.0 ) {
            curIdx++;
            scaleErr--;
        }

       // qDebug() << "MPP: RenderScale:" << scale << curIdx << curPoint << hgt;
    }

    return m_path;

}



void MotionPathPainter::paramsChanged() {
    qDebug() << "MPP: Got parameter changes" << this;
    setMotionCurve();
}

/* These methods are pulled from the OMMotor implementation on AVR, so that we model the
   same move algorithm */



float MotionPathPainter::_linearEasing(float p_tmPos) {

  float curSpd;

  if( p_tmPos <= m_splinePlanned.acTm ) {
    curSpd = m_splinePlanned.topSpeed * ( p_tmPos / m_splinePlanned.acTm);
  }
  else if( p_tmPos <= m_splinePlanned.dcStart ) {
    curSpd = m_splinePlanned.topSpeed;
  }
  else {
    curSpd = m_splinePlanned.topSpeed * (1.0 - ((p_tmPos - m_splinePlanned.acTm - m_splinePlanned.crTm) / m_splinePlanned.dcTm));
  }

  return curSpd;

}




float MotionPathPainter::_qEaseCalc(float p_tmPos) {
  float curSpd;



  if( p_tmPos < m_splinePlanned.acTm ) {
    p_tmPos = p_tmPos / m_splinePlanned.acTm;
    curSpd = m_splinePlanned.topSpeed * p_tmPos * p_tmPos;
  }
  else if( p_tmPos <= m_splinePlanned.dcStart ) {
    curSpd = m_splinePlanned.topSpeed;
  }
  else {
    p_tmPos = 1.0 - (p_tmPos - m_splinePlanned.acTm - m_splinePlanned.crTm) / m_splinePlanned.dcTm;
    curSpd = m_splinePlanned.topSpeed * p_tmPos * p_tmPos;
  }

  return curSpd;

}

float MotionPathPainter::_qInvCalc(float p_tmPos) {
  float curSpd;

  if( p_tmPos < m_splinePlanned.acTm ) {
    p_tmPos = 1.0 - (p_tmPos / m_splinePlanned.acTm);
    curSpd = m_splinePlanned.topSpeed - (m_splinePlanned.topSpeed * p_tmPos * p_tmPos);
  }
  else if( p_tmPos <= m_splinePlanned.dcStart ) {
    curSpd = m_splinePlanned.topSpeed;
  }
  else {
    p_tmPos = (p_tmPos - m_splinePlanned.acTm - m_splinePlanned.crTm) / m_splinePlanned.dcTm;
    curSpd = m_splinePlanned.topSpeed - (m_splinePlanned.topSpeed * p_tmPos * p_tmPos);
  }

  return curSpd;

}

 // pre-calculate spline values to optimize execution time when requesting the
 // velocity at a certain point

void MotionPathPainter::_initSpline(unsigned long p_Steps, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel, unsigned long p_totalSplines) {


   // voodoo
   // TODO: Fix OMMotor, so that we don't use this
   // voodoo.

    if( m_axis->easing == OM_MOT_LINEAR )
      m_splinePlanned.travel = MPP_TRAVEL_LIN;
    else if( m_axis->easing == OM_MOT_QUAD )
      m_splinePlanned.travel = MPP_TRAVEL_QUAD;
    else
      m_splinePlanned.travel = MPP_TRAVEL_QUADINV;

    // pre-calculate values for spline interpolation
   m_splinePlanned.acTm = (float) p_Accel / (float) p_Time;
   m_splinePlanned.dcTm = (float) p_Decel / (float) p_Time;
   m_splinePlanned.crTm = 1.0 - (m_splinePlanned.acTm + m_splinePlanned.dcTm);
   m_splinePlanned.dcStart = m_splinePlanned.acTm + m_splinePlanned.crTm;

   float velocity = (float) p_Steps / (m_splinePlanned.acTm/m_splinePlanned.travel + m_splinePlanned.crTm + m_splinePlanned.dcTm/m_splinePlanned.travel);
   m_splinePlanned.topSpeed = (velocity * m_splinePlanned.crTm) / ( p_totalSplines * m_splinePlanned.crTm );


   // qDebug() << "MPP: IS: " << m_splinePlanned.acTm << m_splinePlanned.dcTm << m_splinePlanned.crTm << m_splinePlanned.dcStart << m_splinePlanned.topSpeed;
}

