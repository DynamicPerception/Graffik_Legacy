#include "motionpathpainter.h"

MotionPathPainter::MotionPathPainter() {
    m_path = new QPainterPath;
}

MotionPathPainter::~MotionPathPainter() {
    delete m_path;
}

void MotionPathPainter::setMotionPath(unsigned short p_addr, OMfilmParams *p_parms, int p_height, int p_width) {
  m_parms = p_parms->axes.value(p_addr);
  delete m_path;

  m_path = new QPainterPath;

  int ms_per_xpix = p_parms->realLength / p_width;

  int splines = (p_parms->realLength - m_parms->startTm - m_parms->endTm) * ms_per_xpix;
  int startPoint = m_parms->startTm * ms_per_xpix;

  _initSpline(m_parms->endDist, m_parms->startTm - m_parms->endTm, m_parms->accelTm, m_parms->decelTm, splines);

  int spd_per_ypix = ((float)p_height * 0.95) / m_splinePlanned.topSpeed;

  m_path->moveTo(startPoint, 0);
  int curPoint = startPoint;

  for(int i = 1; i <= splines; i++) {
      float tmPos = (float) i / (float) splines;
      float curSpd;

      if( m_parms->easing == OM_MOT_LINEAR )
          curSpd = _linearEasing(tmPos);
      else if( m_parms->easing == OM_MOT_QUAD )
          curSpd = _qEaseCalc(tmPos);
      else
          curSpd = _qInvCalc(tmPos);

      int hgt = spd_per_ypix * curSpd;

      m_path->lineTo(curPoint, hgt);
      curPoint++;
  }

}

QPainterPath* MotionPathPainter::getPath() {

    return m_path;
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

void MotionPathPainter::_initSpline(float p_Steps, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel, unsigned long p_totalSplines) {




        // voodoo
        // TODO: Fix OMMotor, so that we don't use this
        // voodoo.

   if( m_parms->easing == OM_MOT_LINEAR )
       m_splinePlanned.travel = MPP_TRAVEL_LIN;
   else if( m_parms->easing == OM_MOT_QUAD )
       m_splinePlanned.travel = MPP_TRAVEL_QUAD;
   else
       m_splinePlanned.travel = MPP_TRAVEL_QUADINV;


    // pre-calculate values for spline interpolation
   m_splinePlanned.acTm = (float) p_Accel / (float) p_Time;
   m_splinePlanned.dcTm = (float) p_Decel / (float) p_Time;
   m_splinePlanned.crTm = 1.0 - (m_splinePlanned.acTm + m_splinePlanned.dcTm);
   m_splinePlanned.dcStart = m_splinePlanned.acTm + m_splinePlanned.crTm;

   float velocity = p_Steps / (m_splinePlanned.acTm/m_splinePlanned.travel + m_splinePlanned.crTm + m_splinePlanned.dcTm/m_splinePlanned.travel);
   m_splinePlanned.topSpeed = (velocity * m_splinePlanned.crTm) / ( p_totalSplines * m_splinePlanned.crTm );


}

