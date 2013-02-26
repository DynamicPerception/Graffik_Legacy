/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

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
    m_wasMax    = 0;
    m_startPx   = 0;
    m_endPx     = 0;
    m_acEndPx   = 0;
    m_dcStartPx = 0;
    m_maxHeight = 0;
    m_wasMs     = 1;
    m_maxSpeed  = 0.0;

    m_curveAvail    = false;
    m_new           = false;
    m_relativeScale = false;

        // fill arrays with default (0) values.
    for(unsigned long i = 0; i < m_maxPoints; i++ ) {
        m_renderPoints.append(0);
        m_stepsTaken.append(0);
    }

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(paramsChanged()));

}

MotionPathPainter::~MotionPathPainter() {
    delete m_path;
}



/** Get X Pixel Point at (Wall) Time

  Returns the X-pixel value of the specified film time
  */

int MotionPathPainter::getX(unsigned long p_time) {
    OMfilmParams parms = m_film->getParamsCopy();
    int x = ((float) p_time / (float) parms.realLength) * (float) m_wasWidth;
    return x;
}

/** Get Film (Wall) Time at pixel

  Returns the time, in milliseconds, of the film's real length (wall time)
  at the specific point represented by x point p_x.

  */

float MotionPathPainter::getFilmTime(int p_x) {

    OMfilmParams parms = m_film->getParamsCopy();
    float filmTm = (((float) parms.realLength / (float)m_wasWidth) * (float)p_x);
    filmTm = filmTm > parms.realLength ? parms.realLength : filmTm;
    return filmTm;
}


/**
    Return whether or not the PainterPath contents changed in the last
    call to getPath().  This will automatically reset after calling this
    method.

    */

bool MotionPathPainter::hasChanged() {
    bool ret = m_hasChanged;
    m_hasChanged = false;
    return ret;
}

/** Return whether or not any curve is actually drawn */

bool MotionPathPainter::isDrawn() {
    return m_curveAvail;
}

/** Get the maximum height of the drawn motion curve */

int MotionPathPainter::getMaxHeight() {
    return m_maxHeight;
}

/** Get the pixel point at which acceleration ends */

int MotionPathPainter::getAcEndPx() {
    return m_acEndPx;
}

/** Get the pixel point at which deceleration begins */

int MotionPathPainter::getDcStartPx() {
    return m_dcStartPx;
}

/** Get the pixel point at which the move starts */

int MotionPathPainter::getStartPx() {
    return m_startPx;
}

/** Get the pixel point at which the move ends */

int MotionPathPainter::getEndPx() {
    return m_endPx;
}

/** Get the maximum speed reached in the profiled move */
float MotionPathPainter::getMaxSpeed() {
    return m_maxSpeed;
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

/** Get the Maximum Number of Curve Points

    Note: this returns the maximum -possible- curve points.
    Actual maximum may be limited by total number of milliseconds
    in movement
*/

unsigned long MotionPathPainter::maxSplinePoints() {
    return m_maxPoints;
}

/** Return the speed (in steps/second) of the Motor at this X position */

float MotionPathPainter::getSpeed(unsigned long p_x) {

    if( m_wasDist == 0 )
        return 0.0;

    OMfilmParams filmParams = m_film->getParamsCopy();
        // determine how many seconds per sample...
    float mult = 1000.0 / ((float) filmParams.realLength / _getMaxPoints());
    unsigned long pos = (float) p_x * _getScale();

    return m_renderPoints[pos] * mult;
}

/** Return the distance from home (in steps) of the Motor at this X Position */

float MotionPathPainter::getPosition(unsigned long p_x) {

  if( m_wasDist == 0 )
      return 0.0;

  unsigned long pos = (float) p_x * _getScale();
  return m_stepsTaken[pos];

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
            axParms->ms == m_wasMs &&
            filmParams.realLength == m_wasLength)
        return;


    qDebug() << "MPP: Running new curve";

        // wipe out all saved points

    m_renderPoints.clear();
    m_stepsTaken.clear();

    m_wasDist   = axParms->endDist;
    m_wasEnd    = axParms->endTm;
    m_wasStart  = axParms->startTm;
    m_wasAcc    = axParms->accelTm;
    m_wasDcc    = axParms->decelTm;
    m_wasEase   = axParms->easing;
    m_wasMs     = axParms->ms;

    m_new        = true;
    m_curveAvail = false;

    float  stepMod = 1.0;
    long moveSteps = axParms->endDist;

            // get absolute value and save modifier
    if( moveSteps < 0 ) {
        moveSteps *= -1;
        stepMod = -1.0;
    }

    if( filmParams.realLength == 0 )
        return;


        // no movement, so just walk away now
    if( moveSteps < 1 )
        return;

    unsigned long maxSpeed = m_aopt->getOptions(m_addr)->maxSteps;
    bool              sane = true;
                m_maxSpeed = 0.0;
              m_curveAvail = true;

        // put upper boundary at 1 point per millisecond
    unsigned long actMaxPts = _getMaxPoints();

        // if no end time is specified, then the move ends when the film does
    unsigned long endTm = axParms->endTm > 0 ? axParms->endTm : filmParams.realLength;
    float      travelTm = endTm - axParms->startTm;


        // determine padding to add before and after move

    float       ms_per_xpt = (float) filmParams.realLength / (float) actMaxPts;
    unsigned long jmpAhead = (float) axParms->startTm / (float) ms_per_xpt;
    unsigned long    endPt = endTm / (float) ms_per_xpt;


    endPt = endPt > actMaxPts ? actMaxPts : endPt;

    unsigned long   leave = endPt > 0 ? actMaxPts - endPt : 0;
    unsigned long plotPts = actMaxPts - jmpAhead - leave;

        // pad array prior to move
    for( unsigned long i = 0; i < jmpAhead; i++) {
        m_renderPoints.append(0);
        m_stepsTaken.append(0);
    }


//    qDebug() << "MPP: INIT:" << filmParams.realLength << axParms->startTm << axParms->accelTm << m_splinePlanned.acTm << travelTm << endTm << jmpAhead << leave << plotPts << m_wasWidth;

    _initSpline(moveSteps, travelTm, axParms->accelTm, axParms->decelTm, plotPts);

    float totalSteps = 0.0;

    for(unsigned long i = 1; i <= plotPts; i++) {


          float tmPos = (float) i / (float) plotPts;
          float curSpd = 0.0;

          if( axParms->easing == OM_MOT_LINEAR )
              curSpd = _linearEasing(tmPos);
          else if( axParms->easing == OM_MOT_QUAD )
              curSpd = _qEaseCalc(tmPos);
          else
              curSpd = _qInvCalc(tmPos);

          m_maxSpeed = curSpd > m_maxSpeed ? curSpd : m_maxSpeed;

          m_renderPoints.append(curSpd);

            // record steps taken from home
          totalSteps += curSpd;
          m_stepsTaken.append(totalSteps * stepMod);
    }

        // pad array after move
    for( unsigned long i = 0; i < leave; i++ ) {
        m_renderPoints.append(0);
        m_stepsTaken.append(totalSteps * stepMod);
    }

        // block movement of the axis if it tries to exceed its maximum speed,
        // or the total step rate exceeds the ability of the driver

    m_aopt->removeError(m_addr, AxisErrors::ErrorSpeedExceeded);

    if( m_maxSpeed > ((float)maxSpeed / (1000.0 / ms_per_xpt)) || ( m_maxSpeed * (1000.0 / ms_per_xpt)) * m_wasMs > MPP_REAL_MAX ) {
        qDebug() << "MPP: Motion Profile is not Sane";
        m_aopt->error(m_addr, AxisErrors::ErrorSpeedExceeded);
        sane = false;
    }

    emit moveSane(sane);
}

/** Get Path Model

  Returns a QPainterPath representing the movement curve, scaled to
  the supplied QRect.

  If the QRect doesn't change its width or height, and no changes to the curve have been made,
  no action will be   taken and the pre-existing QPainterPath will be returned.  If it has
  changed, a new path will be generated from existing curve data, scaled to the current QRect.

  */

QPainterPath* MotionPathPainter::getPath(QRect p_area) {

    m_area = p_area;

    OMfilmParams filmParams = m_film->getParamsCopy();
    OMaxisOptions*    aopts = m_aopt->getOptions(m_addr);

        // do nothing if we've already drawn this
    if( p_area.height() == m_wasHeight &&
            p_area.width() == m_wasWidth &&
            m_new == false &&
            filmParams.realLength == m_wasLength &&
            aopts->maxSteps == m_wasMax )
        return m_path;

    unsigned int  width = p_area.width();
    unsigned int height = p_area.height();

        // set a new floor
    height = height * MPP_HEIGHT_BUF;

    m_hasChanged = true;
           m_new = false;
      m_wasWidth = p_area.width();
     m_wasHeight = p_area.height();
     m_wasLength = filmParams.realLength;
        m_wasMax = aopts->maxSteps;

        // put upper boundary at 1 point per millisecond
    unsigned long actMaxPts = m_maxPoints > filmParams.realLength ? filmParams.realLength : m_maxPoints;

        // determine how to scale the raw data for this sized
        // display
    float scale = _getScale();
        // we can't have fractional points in array, so we need
        // to accumulate and overflow error
    int scaleMajor = int(scale);
    scale -= scaleMajor;
    float scaleErr = 0.0;

        // maxSteps in axis options is steps/second...
        // so we need seconds per (horizontal, using all points) pixel

    float spd_per_ypix = 0.0;

    if( m_relativeScale == true ) {
        float pixelSecs = ((float)filmParams.realLength / 1000.0) / (float)actMaxPts;
        spd_per_ypix = height / (pixelSecs * (float) aopts->maxSteps);
    }
    else {
            // set a limit at 95% high for proportional displa
        spd_per_ypix = (height * MPP_HEIGHT_BUF) / (float) m_splinePlanned.topSpeed;
    }

        // flush out the path, and create a new one
    delete m_path;
    m_path = new QPainterPath;

    m_maxHeight = height;

    int   curIdx = 0;
    int isMoving = 0;

    m_path->moveTo(0, height);

        // for each pixel in the display,
    for( unsigned long i = 0; i < width; i++ ) {

            // it's possible, but unlikely, that our scale
            // could move us past the end of our available points
        if( curIdx > m_renderPoints.count() - 1 )
            break;

        float curSpd = m_renderPoints[curIdx];

        int hgt = (float) height - (spd_per_ypix * (float) curSpd);

        m_maxHeight = hgt < m_maxHeight ? hgt : m_maxHeight;

        m_path->lineTo(i, hgt);

            // find key pixel points in curve and store them for
            // querying later

        if( isMoving == 0 && curSpd > 0.0 ) {
            m_startPx = i;
            isMoving = 1;
        }
        else if( isMoving == 1 && _inFloatRange(curSpd, m_splinePlanned.topSpeed)) {
            m_acEndPx = i;
            isMoving = 2;
        }
        else if( isMoving == 2 && ! _inFloatRange(curSpd, m_splinePlanned.topSpeed)) {
            m_dcStartPx = i;
            isMoving = 3;
        }
        else if( isMoving == 3 && curSpd <= 0.0 ) {
            m_endPx = i;
            isMoving = 4;
        }


            // move point in source data ahead according to scale and accumulate any scaling error
        curIdx += scaleMajor;
        scaleErr += scale;

            // if scaling error overflows, move one point ahead
        if( scaleErr >= 1.0 ) {
            curIdx++;
            scaleErr--;
        }

    }

        // was the end of movement all the way at the end of the drawn
        // area? If so, indicate it

    if( isMoving != 4 )
        m_endPx = width;

    return m_path;

}



void MotionPathPainter::paramsChanged() {
    qDebug() << "MPP: Got parameter changes" << this;
    setMotionCurve();
}

bool MotionPathPainter::_inFloatRange(float p_left, float p_right) {
    if( p_left < p_right + 0.00000001 && p_left > p_right - 0.00000001 )
        return true;

    return false;
}

unsigned long MotionPathPainter::_getMaxPoints() {
    OMfilmParams filmParams = m_film->getParamsCopy();
    // put upper boundary at 1 point per millisecond
    unsigned long actMaxPts = m_maxPoints > filmParams.realLength ? filmParams.realLength : m_maxPoints;
    return actMaxPts;
}

float MotionPathPainter::_getScale() {

        // determine how to scale the raw data for this sized
        // display

    return (float) _getMaxPoints() / (float) m_wasWidth;
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
   // TODO: Fix OMMotor, so that we don't use this voodoo.

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

