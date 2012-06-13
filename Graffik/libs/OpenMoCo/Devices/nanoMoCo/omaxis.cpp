#include <QDebug>

#include "omaxis.h"

/*
 *
 *
 * OMAxis Class Methods
 */


/** Constructor

  You must pass an OMMoCoBus object and the address of this device instance

  */

OMAxis::OMAxis(OMMoCoBus* c_bus, unsigned short c_adr) : OMDevice(c_bus, c_adr), SlimScriptDevice() {

   deviceVersion = 1;
   deviceType = "nanoMoCo";

   this->_initScripting();

}

OMAxis::~OMAxis() {

}



/** Enable Camera

 Enables camera, so that it will fire on cycle.

 @return
 The ID of the command

 */

const int OMAxis::cameraEnable() {
   return this->command(COMPROG, progCamOn);

}

/** Disable Camera

 Prevent camera from firing on cycle

 @return
 The ID of the command

 */


const int OMAxis::cameraDisable() {
   return this->command(COMPROG, progCamOff);

}

/** Start Program

 Cause program to begin playing

 @return
 The ID of the command
 */

const int OMAxis::play() {
   return this->command(COMPROG, progPlay);

}

/** Stop Program

 Stop program, re back to beginning, purge state information

 @return
 The ID of the command
 */

const int OMAxis::stop() {
   return this->command(COMPROG, progStop);

}

/** Pause Program

 Pause current program execution, allows starting again without
 loss of program information

 @return
 The ID of the command
 */

const int OMAxis::pause() {
   return this->command(COMPROG, progPause);

}

/** Enable or Disable Continuous Motion

  Enables or disables continuous motion, if continuous motion is
  enabled, control the continuous speed with the speed() method,
  and use move(dir, 0) to start movement.  You may continuously
  alter the speed of travel by calling speed() with a new speed
  during the movement.

 */

const int OMAxis::continuous(bool en) {
    return this->command(COMDATA, dataMot, motContEn, (char) en);
}

/** Move Motor Now

 Moves motor a given number of steps immediately in the given direction.
 Specifying steps as '0' causes motor to engaged in continuous motion
 (no defined stop point).  Use stopMotor() and speed() to control continuous
 behavior.


 @param dir
 Direction

 @param steps
 Num of steps to take

 @return
 The ID of the command
 */

const int OMAxis::move(bool dir, unsigned short steps) {
   return this->command(COMPROG, progMove, dir, steps);
}

/** Move Motor Now

   Moves motor immediately, with easing.

   You must specify the direction to move, the number of steps to move,
   the time to arrive at the destination (in milliseconds), the time to
   accelerate (in ms), and the time to decelerate (in ms).

   @param dir
   Direction

   @param step
   Total steps

   @param arrive
   Arrival time (in mS)

   @param accel
   Accel time (in mS)

   @param decel
   Decel time (in mS)

   @return
   The ID of the command
   */

const int OMAxis::move(bool dir, unsigned long step, unsigned long arrive, unsigned long accel, unsigned long decel) {

    char* data = new char[20]();

        // place in direction and spacer bytes
    data[0] = dir;
    data[5] = 32;
    data[10] = 32;
    data[15] = 32;


    char* dPtr = data;
    dPtr += 1;

        // copy steps into place
    char* cD = nwo(step);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy arrive time
    dPtr += 5;
    cD = nwo(arrive);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy accel time
    dPtr += 5;
    cD = nwo(accel);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy decel time
    dPtr += 5;
    cD = nwo(decel);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

   return this->command(COMPROG, progMoveA, data, 20);
   delete[] data;
}

/** Plan Motion

   Plans a complex, eased move across a shot cycle (for automated
   shoot-move-shoot cycles).

   You must specify the direction to move, the number of steps to move,
   the number of shots to take, the count of shots over which acceleration
   occurs, and the count of shots over which deceleration occurs.

   @param dir
   Direction

   @param step
   Total steps

   @param shots
   Total shot cycles

   @param accel
   Acceleration shot cycles

   @param decel
   Deceleration shot cycles

   @return
   The ID of the command
   */

const int OMAxis::plan(bool dir, unsigned long step, unsigned long shots, unsigned long accel, unsigned long decel) {

    char* data = new char[20]();

        // place in direction and spacer bytes
    data[0] = dir;
    data[5] = 32;
    data[10] = 32;
    data[15] = 32;


    char* dPtr = data;
    dPtr += 1;

        // copy steps into place
    char* cD = nwo(step);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy shot count
    dPtr += 5;
    cD = nwo(shots);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy accel time
    dPtr += 5;
    cD = nwo(accel);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

        // copy decel time
    dPtr += 5;
    cD = nwo(decel);
    memcpy((void*) dPtr, cD, 4);
    delete[] cD;

   return this->command(COMPROG, progPlan, data, 20);
   delete[] data;
}

/** Set Easing Algorithm

 Sets the current easing algorithm in use

 @param p_ease
 The easing algorithm, one of: OM_MOT_LINEAR, OM_MOT_QUAD, or OM_MOT_QUADINV

 @return
 The ID of the command
 */

const int OMAxis::easing(unsigned char p_ease) {
   return this->command(COMDATA, dataMot, motEase, (char) p_ease);

}

/** Stop Motor Movement

 Stop continuous motion movement, when not running in
 program mode.

 @return
 The ID of the command
 */

const int OMAxis::stopMotor() {
   return this->command(COMPROG, progStpMot);

}

/** Send Motor Home

 Sends motor to its set home position

 @return
 The ID of the command
 */

const int OMAxis::home() {
   return this->command(COMPROG, progHome);
}

/** Set Current Position as Home

  Sets current motor position as home.

  @return
  The ID of the command
  */

const int OMAxis::setHome() {
    return this->command(COMDATA, dataMot, motHome);
}

/** Set Max Stepping Rate

 Specifies Maximum Stepping rate

 @param p_Rate
 Stepping rate, one of the following: 10000, 5000, 4000, 2000, or 1000

 @return
 The ID of the command
 */

const int OMAxis::maxStepRate(unsigned short p_Rate) {
   return this->command(COMPROG, progMaxRate, p_Rate);
}

/** Set Max Stepping Speed

 Specifies Maximum Stepping Speed in steps/second, used
 to set a lower speed than the selections for maxStepRate()
 allow.

 @param p_Spd
 Steps/second

 @return
 The ID of the command
 */

const int OMAxis::maxStepSpeed(unsigned short p_Spd) {
   return this->command(COMPROG, progMaxSpeed, p_Spd);
}

/** MicroStep Control

 Specifies current microstep value

 @param steps
 Microsteps (1, 2, 4, 8, or 16)

 @return
 The ID of the command
 */

const int OMAxis::microSteps(unsigned char steps) {
   return this->command(COMPROG, progMotMs, (char) steps);
}

/** Control Motor Sleep

 Enable or disable motor sleep between moves

 @param value
 Enable (true) or Disable (false)

 @return
 The ID of the command
 */

const int OMAxis::sleep(bool value) {
   return this->command(COMPROG, progKill, (char) value);
}



/** Expose Camera Now

 Take an exposure immediately, even if the program is
 not running or the camera is disabled.

 @param tm
 Exposure time in milliseconds

 @return
 The ID of the command
 */

const int OMAxis::expose(unsigned long tm) {
   return this->command(COMPROG, progShoot, tm);
}

/** Control Debug Line/LED

 Enables or disables pulsing of the debug line/LED.

 @param value
 Enable (true) or disable (false) the debug line

 @return
 The ID of the command
 */

const int OMAxis::debug(bool value) {
   return this->command(COMPROG, progDbg, (char) value);
}

/** Motor Steps

 Sets the # of steps to take during each cycle

 @param stps
 Number of steps to take between moves

 @return
 The ID of the command
 */

const int OMAxis::steps(unsigned short stps) {
   return this->command(COMDATA, dataMot, motSteps, stps);
}

/** Motor Ramp Cycles

 Sets the # of shot cycles to ramp up to full speed

 @param cycles
 Number of cycles to accelerate over

 @return
 The ID of the command
 */

const int OMAxis::rampSteps(unsigned char cycles) {
   return this->command(COMDATA, dataMot, motRamp, (char) cycles);
}

/** Motor Direction

 Sets the current direction

 @param dir
 Direction

 @return
 The ID of the command
 */

const int OMAxis::direction(bool dir) {
   return this->command(COMDATA, dataMot, motDir, (char) dir);
}

/** Maximum Step Travel

 Sets the maximum # of steps the motor should take
 before stopping.

 @param stps
 Maximum # of steps

 @return
 The ID of the command
 */

const int OMAxis::maxSteps(unsigned long stps) {
   return this->command(COMDATA, dataMot, motMax, stps);
}

/** Continuous Motion Speed

 Sets the current continuous motion speed in steps/second

 @param stps
 Steps to take per second

 @return
 The ID of the command
 */

const int OMAxis::speed(float p_Steps) {
   return this->command(COMDATA, dataMot, motCont, p_Steps);
}


/** Interval Seconds

 Sets the camera intervalometer timing in seconds

 @param seconds
 Interval Seconds

 @return
 The ID of the command
 */

const int OMAxis::interval(unsigned short seconds) {
   return this->command(COMDATA, dataCam, camInt, seconds);
}

/** Exposure Time

 Sets the camera exposure time (how long trigger line is held high)

 @param ms
 Exposure time in milliseconds

 @return
 The ID of the command
 */

const int OMAxis::exposure(unsigned long ms) {
   return this->command(COMDATA, dataCam, camExp, ms);
}

/** Focus Tap Time

 Sets the camera focus tap time (how long the focus line is held high before an exposure)

 @param ms
 Focus time in milliseconds

 @return
 The ID of the command
 */

const int OMAxis::focus(unsigned short ms) {
   return this->command(COMDATA, dataCam, camFoc, ms);
}

/** Max Shots

 Sets the maximum # of shots to take (program will stop
 when this number of shots have been taken)

 @param shots
 Number of shots to take

 @return
 The ID of the command
 */

const int OMAxis::maxShots(unsigned short shots) {
   return this->command(COMDATA, dataCam, camMax, shots);
}

/** Exposure Delay

 Sets the camera exposure delay time (how long the program waits after the shutter has been brought high)

 @param ms
 Delay time in milliseconds

 @return
 The ID of the command
 */

const int OMAxis::exposureDelay(unsigned short ms) {
   return this->command(COMDATA, dataCam, camPost, ms);
}

/** Focus with Shutter

 Enables or disables connecting the focus line when the shutter line is
 connected.  This, effectively, engages the focus whenever the shutter is
 engaged, as is needed with some cameras (especially Nikons).

 @param foc
 Enable (true) or disable (false) the focus with shutter setting

 @return
 The ID of the command
 */

const int OMAxis::focusShutter(bool foc) {
   return this->command(COMDATA, dataCam, camFWS, (char) foc);
}

/** Camera Repeat count

 Sets the camera cycle repeat count (how many photos to take per
 interval cycle. At each point in the interval, it will take this
 many photos, with each photo repeating the normal photo setting.
 The ability to trigger actions between each photo makes this feature
 more useful for gigapan and similar photo styles.)

 @param count
 Count of photos to take per cycle.

 @return
 The ID of the command
 */

const int OMAxis::exposureRepeat(unsigned char count) {
   return this->command(COMDATA, dataCam, camRpt, (char) count);
}



/** Status: Interval

 Retrieves the current interval time.


 @return
 The ID of the command
 */

const int OMAxis::getInterval() {

   return this->command(COMPROG, progStat, statInterval);
}

/** Status: Exposure Length

 Retrieves the current exposure length time.


 @return
 The ID of the command
 */

const int OMAxis::getExposure() {

   return this->command(COMPROG, progStat, statExp);
}

/** Status: Exposure Delay

 Retrieves the current post exposure delay time.


 @return
 The ID of the command
 */

const int OMAxis::getExposureDelay() {

   return this->command(COMPROG, progStat, statPost);
}

/** Status: Shots Fired

 Retrieves the current number of shots fired.


 @return
 The ID of the command
 */

const int OMAxis::getShotCount() {

   return this->command(COMPROG, progStat, statShots);
}

/** Status: Motor Direction

 Retrieves the current direction of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getDirection() {

   return this->command(COMPROG, progStat, statMotDir);
}

/** Status: Motor Ramp Value

 Retrieves the current motor ramp setting of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getRampSteps() {

   return this->command(COMPROG, progStat, statMotRamp);
}


/** Status: Motor Enable

 Retrieves the current motor enable setting of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getMotorEnable() {

   return this->command(COMPROG, progStat, statMotEn);
}

/** Status: Motor Steps

 Retrieves the current motor steps (between shots) setting of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getSteps() {

   return this->command(COMPROG, progStat, statMotSteps);
}

/** Status: Motor Home Distance

 Retrieves the current distance from home of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getHomeDist() {

   return this->command(COMPROG, progStat, statMotHmDist);
}

/** Status: Motor Steps Moved

 Retrieves the total steps moved of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getStepsMoved() {

   return this->command(COMPROG, progStat, statMotMoved);
}

/** Status: Motor Backlash

 Retrieves the backlash setting of the given axis.


 @return
 The ID of the command
 */

const int OMAxis::getBacklash() {

   return this->command(COMPROG, progStat, statMotBack);
}

/** Status: Camera Enabled

 Retrieves the camera enabled setting value.


 @return
 The ID of the command
 */

const int OMAxis::getCameraEnable() {

   return this->command(COMPROG, progStat, statCamera);
}


/** Status: Run Time

 Retrieves the current program run time in mS.


 @return
 The ID of the command
 */

const int OMAxis::getRunTime() {

   return this->command(COMPROG, progStat, statTime);
}

/** Status: Currently Exposing

 Retrieves the current exposing value.  True if
 camera is exposing, false if not.


 @return
 The ID of the command
 */

const int OMAxis::getExposing() {

   return this->command(COMPROG, progStat, statExposing);
}


/** Enable the Motor

  @return
  The ID of the command
  */

const int OMAxis::motorEnable() {
    return this->command(COMDATA, dataMot, motEnable);
}

/** Disable the Motor

  @return
  The ID of the command
  */

const int OMAxis::motorDisable() {
    return this->command(COMDATA, dataMot, motDisable);
}


/** Set Timing Node

  One node on the bus may control timing for all other nodes
  by sending a "clear to execute" signal at the appropriate
  time.  This allows multiple nodes to synchronize with
  each other.  Only one node on the bus may be the timing
  master.

  @param p_mast
  True if this node should be the timing master, false if not.

  @return
  The ID of the command
  */

const int OMAxis::timing(bool p_mast) {
    return this->command(COMDATA, dataMot, motDisable);
}

void OMAxis::_initScripting() {

   this->addNamedCommand("start", static_cast<f_callBack>(&OMAxis::_slimStart));
   this->addNamedCommand("stop", static_cast<f_callBack>(&OMAxis::_slimStop));
   this->addNamedCommand("pause", static_cast<f_callBack>(&OMAxis::_slimPause));
   this->addNamedCommand("motor", static_cast<f_callBack>(&OMAxis::_slimMotor));
   this->addNamedCommand("debug", static_cast<f_callBack>(&OMAxis::_slimLed));
   this->addNamedCommand("dir", static_cast<f_callBack>(&OMAxis::_slimDir));
   this->addNamedCommand("expose", static_cast<f_callBack>(&OMAxis::_slimExpose));
   this->addNamedCommand("interval", static_cast<f_callBack>(&OMAxis::_slimInterval));
   this->addNamedCommand("max", static_cast<f_callBack>(&OMAxis::_slimMax));
   this->addNamedCommand("steps", static_cast<f_callBack>(&OMAxis::_slimSteps));
   this->addNamedCommand("camera", static_cast<f_callBack>(&OMAxis::_slimCamera));
   this->addNamedCommand("exposure", static_cast<f_callBack>(&OMAxis::_slimExposure));
   this->addNamedCommand("delay", static_cast<f_callBack>(&OMAxis::_slimExposureDelay));
   this->addNamedCommand("focus", static_cast<f_callBack>(&OMAxis::_slimFocus));
   this->addNamedCommand("speed", static_cast<f_callBack>(&OMAxis::_slimSpeed));
   this->addNamedCommand("ms", static_cast<f_callBack>(&OMAxis::_slimMs));
   this->addNamedCommand("move", static_cast<f_callBack>(&OMAxis::_slimMove));
   this->addNamedCommand("tie", static_cast<f_callBack>(&OMAxis::_slimTie));
   this->addNamedCommand("repeat", static_cast<f_callBack>(&OMAxis::_slimRepeat));
   this->addNamedCommand("status", static_cast<f_callBack>(&OMAxis::_slimStatus) );
   this->addNamedCommand("plan", static_cast<f_callBack>(&OMAxis::_slimPlan) );
   this->addNamedCommand("easing", static_cast<f_callBack>(&OMAxis::_slimEasing) );
   this->addNamedCommand("address", static_cast<f_callBack>(&OMAxis::_slimAddr) );
   this->addNamedCommand("home", static_cast<f_callBack>(&OMAxis::_slimHome) );
   this->addNamedCommand("sethome", static_cast<f_callBack>(&OMAxis::_slimSetHome) );
   this->addNamedCommand("master", static_cast<f_callBack>(&OMAxis::_slimMaster) );
   this->addNamedCommand("continuous", static_cast<f_callBack>(&OMAxis::_slimContinuous) );
    this->addNamedCommand("stopmotor", static_cast<f_callBack>(&OMAxis::_slimStopMotor) );
    this->addNamedCommand("sleep", static_cast<f_callBack>(&OMAxis::_slimSleep) );

}

const int OMAxis::_slimMax(QStringList& p_str) {
    if( p_str.count() < 2 )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "shots" )
        return maxShots( p_str[1].toUShort() );
    else if( p_str[0] == "speed" )
        return maxStepSpeed( p_str[1].toUShort() );
    else if( p_str[0] == "steps" )
        return maxSteps( p_str[1].toULong() );
    else if( p_str[0] == "rate" )
        return maxStepRate( p_str[1].toUShort() );
    else
        throw SLIM_ERR_ARG;

}

const int OMAxis::_slimStart(QStringList& p_str) {
    return play();
}

const int OMAxis::_slimStop(QStringList& p_str) {
    return stop();
}

const int OMAxis::_slimStopMotor(QStringList& p_str) {
    return stopMotor();
}
const int OMAxis::_slimPause(QStringList& p_str) {
    return pause();
}

const int OMAxis::_slimHome(QStringList& p_str) {
    return home();
}

const int OMAxis::_slimSetHome(QStringList& p_str) {
    return setHome();
}

const int OMAxis::_slimSleep(QStringList & p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "enable" ) {
        return sleep(true);
    }
    else if( p_str[0] == "disable" ) {
        return sleep(false);
    }
    else {
        throw SLIM_ERR_ARG;
    }

}

 // motor on|off slim command
const int OMAxis::_slimMotor(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "enable" ) {
        return motorEnable();
    }
    else if( p_str[0] == "disable" ) {
        return motorDisable();
    }
    else {
        throw SLIM_ERR_ARG;
    }

}

// motor continuous slim command
const int OMAxis::_slimContinuous(QStringList& p_str) {

   if( p_str.isEmpty() )
       throw SLIM_ERR_ARGS;

   if( p_str[0] == "enable" ) {
       return continuous(true);
   }
   else if( p_str[0] == "disable" ) {
       return continuous(false);
   }
   else {
       throw SLIM_ERR_ARG;
   }

}


const int OMAxis::_slimLed(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "on" )
        return debug(true);
    else if( p_str[0] == "off" )
        return debug(false);
    else
        throw SLIM_ERR_ARG;



}

const int OMAxis::_slimDir(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "true" || p_str[0] == "1" )
        return direction(true);
    else if( p_str[0] == "false" || p_str[0] == "0" )
        return direction(false);
    else
        throw SLIM_ERR_ARG;

}

const int OMAxis::_slimMaster(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "true" || p_str[0] == "1" )
        return timing(true);
    else if( p_str[0] == "false" || p_str[0] == "0" )
        return timing(false);
    else
        throw SLIM_ERR_ARG;

}

const int OMAxis::_slimExpose(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return expose(p_str[0].toULong());
}

const int OMAxis::_slimSteps(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return steps(p_str[0].toULong());
}

const int OMAxis::_slimInterval(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return interval(p_str[0].toUShort());
}

const int OMAxis::_slimCamera(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "enable" )
        return cameraEnable();
    else if( p_str[0] == "disable" )
        return cameraDisable();
    else
        throw SLIM_ERR_ARG;


}

const int OMAxis::_slimExposure(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return exposure(p_str[0].toULong());
}

const int OMAxis::_slimExposureDelay(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return exposureDelay(p_str[0].toUShort());
}

const int OMAxis::_slimFocus(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return focus(p_str[0].toUShort());
}

const int OMAxis::_slimTie(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "enable" )
        return focusShutter(true);
    else if( p_str[0] == "disable" )
        return focusShutter(false);
    else
        throw SLIM_ERR_ARG;
}

const int OMAxis::_slimRepeat(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return exposureRepeat(p_str[0].toUShort());
}

const int OMAxis::_slimSpeed(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    return speed(p_str[0].toFloat());
}


const int OMAxis::_slimMs(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    unsigned char arg = p_str[0].toUShort();

        // only allowed values
//    if( arg != 1 || arg != 2 || arg != 4 || arg != 8 || arg != 16 )
//        throw SLIM_ERR_ARG;

    return microSteps(arg);
}

const int OMAxis::_slimMove(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    int argCnt = p_str.count();

    if( argCnt < 2 || (argCnt > 2 && argCnt != 5) )
        throw SLIM_ERR_ARGS;

    bool dir = false;

    if( p_str[0] == "true" || p_str[0] == "1" )
        dir = true;

    if( argCnt == 2 )
        return move( dir, p_str[1].toULong() );
    else
        return move( dir, p_str[1].toULong(), p_str[2].toULong(), p_str[3].toULong(), p_str[4].toULong() );

    throw SLIM_ERR_ARG;
}

const int OMAxis::_slimPlan(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    int argCnt = p_str.count();

    if( argCnt != 5 )
        throw SLIM_ERR_ARGS;

    bool dir = false;

    if( p_str[0] == "true" || p_str[0] == "1" )
        dir = true;

        return plan( dir, p_str[1].toULong(), p_str[2].toULong(), p_str[3].toULong(), p_str[4].toULong() );

    throw SLIM_ERR_ARG;
}


const int OMAxis::_slimStatus(QStringList& p_str) {
    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    if( p_str[0] == "interval" )
        return getInterval();
    else if( p_str[0] == "exposure")
        return getExposure();
    else if( p_str[0] == "delay")
        return getExposureDelay();
    else if( p_str[0] == "shots")
        return getShotCount();
    else if( p_str[0] == "dir")
        return getDirection();
    else if( p_str[0] == "motor")
        return getMotorEnable();
    else if( p_str[0] == "home")
        return getHomeDist();
    else if( p_str[0] == "moved")
        return getStepsMoved();
    else if( p_str[0] == "delay")
        return getBacklash();
    else if( p_str[0] == "camera")
        return getCameraEnable();
    else if( p_str[0] == "time")
        return getRunTime();
    else if( p_str[0] == "exposing")
        return getExposing();
    else if( p_str[0] == "version")
        return getVersion();
    else if( p_str[0] == "bus")
        return getBusVer();
    else if( p_str[0] == "id")
        return getId();
    else
        throw SLIM_ERR_ARG;

}

// motor easing mode
const int OMAxis::_slimEasing(QStringList& p_str) {

   if( p_str.isEmpty() )
       throw SLIM_ERR_ARGS;

   if( p_str[0] == "linear" ) {
       return easing(OM_MOT_LINEAR);
   }
   else if( p_str[0] == "quad" ) {
       return easing(OM_MOT_QUAD);
   }
   else if( p_str[0] == "invquad" ) {
       return easing(OM_MOT_QUADINV);
   }
   else {
       throw SLIM_ERR_ARG;
   }

}

const int OMAxis::_slimAddr(QStringList& p_str) {

    if( p_str.isEmpty() )
        throw SLIM_ERR_ARGS;

    uint8_t addr = (uint8_t) p_str[0].toUInt();

    const int id = changeAddress(addr);
    // protected, inherited member
    deviceAddress = addr;
    return id;
}
