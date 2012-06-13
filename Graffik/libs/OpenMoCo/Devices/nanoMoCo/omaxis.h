#ifndef OMAXIS_H
#define OMAXIS_H

#include <QObject>
#include <QStringList>

#include "MoCoBus/ommocobus.h"
#include "MoCoBus/omcommandbuffer.h"
#include "MoCoBus/omdevice.h"
#include "Slim/slimscriptdevice.h"


#define OM_MOT_LINEAR   1
#define OM_MOT_QUAD     2
#define OM_MOT_QUADINV  3

/**

  OMAxis provides the primary interface to a nanomoco engine
  attached to a serial port.

  Each OMAxis must be associated with a controller, by passing an
  OMMoCoBus object when creating an OMAxis object.  You do
  not have to connect before issuing a command, as all commands
  will be queued until the controller is actually connected to
  the serial port.

  The OMAxis object provides a high-level interface to all of the
  available controls for a NanoMoCo driver.  You may issue any
  command to the driver via a provided method.  All commands are
  transmitted and all responses received in a separate thread to
  allow for non-blocking operations.  To receive the result of any
  command you issue, you will need to create a slot to receive the
  complete() signal.

  The following provides a basic example of using an OMAxis object:


  @code

  #include <QDebug>

  ...

  #include "OMMoCoBus.h"
  #include "omaxis.h"
  ...

  int main(int argc, char *argv[])
  {

    ...

    char* serPort[] = "COM5";
    int address = 1234;

    OMMoCoBus* ctrl = new OMMoCoBus(serPort);

    OMAxis* axis = new OMAxis(ctrl, address);

    try {

        ctrl->connect();
    }
    catch (int e) {
        if( e == OpenMoCo::errSerialNotAvailable ) {
            qDebug() << "ERROR: Could not connect to serial port";
        }
        else {

            qDebug() << "ERROR: Caught signal: " << e;
        }

        exit();
    }

    qDebug() << "Successfully connected to " << serPort;

    int cmdId = axis->debug(true);

    qDebug() << "Sent enable debug led command, with id " << cmdId;


  }
  @endcode

  Note that all commands are non-blocking.  This means you must implement
  a slot mechanism to get the result of any command you send.  While this may
  seem somewhat cumbersome, consider for a moment that some commands to an engine
  may be blocking (or, you may be issueing many commands to many engines), and that
  a graphical user interface would not respond during this time - especially if an
  engine were suddenly offline, and you waited for a timeout.

  See the OMAxis::complete() signal documentation for more information.

  @author
  C. A. Church

  */

class OMAxis : public OMDevice, public SlimScriptDevice
{

    Q_OBJECT

public:

    OMAxis(OMMoCoBus*, unsigned short);
    ~OMAxis();

    const int cameraEnable();
    const int cameraDisable();

    const int motorEnable();
    const int motorDisable();

    const int play();
    const int stop();
    const int pause();

    const int home();
    const int setHome();

    const int expose(unsigned long);
    const int sleep(bool);
    const int maxStepRate(unsigned short);
    const int maxStepSpeed(unsigned short);
    const int debug(bool);

    const int continuous(bool);
    const int move(bool, unsigned short);
    const int move(bool, unsigned long, unsigned long, unsigned long, unsigned long);
    const int plan(bool, unsigned long, unsigned long, unsigned long, unsigned long);

    const int easing(unsigned char);

    const int timing(bool);

    const int stopMotor();

    const int steps(unsigned short);
    const int speed(float);
    const int rampSteps(unsigned char);
    const int direction(bool);
    const int maxSteps(unsigned long);
    const int microSteps(unsigned char);
    const int interval(unsigned short);
    const int exposure(unsigned long);
    const int focus(unsigned short);
    const int maxShots(unsigned short);
    const int exposureDelay(unsigned short ms);
    const int focusShutter(bool);
    const int exposureRepeat(unsigned char);
    const int exposureRepeatAction(unsigned char, unsigned char);

    const int getInterval();
    const int getExposure();
    const int getExposureDelay();
    const int getShotCount();
    const int getDirection();
    const int getRampSteps();
    const int getMotorEnable();
    const int getSteps();
    const int getHomeDist();
    const int getStepsMoved();
    const int getBacklash();
    const int getCameraEnable();
    const int getRunTime();
    const int getExposing();







private:

        unsigned short _thsAddr;


        void _initScripting();

        // Script handling functions

        const int _slimStart(QStringList&);
        const int _slimStop(QStringList&);
        const int _slimPause(QStringList&);

        const int _slimLed(QStringList&);
        const int _slimInterval(QStringList&);

        const int _slimCamera(QStringList&);
        const int _slimExpose(QStringList&);
        const int _slimExposure(QStringList&);
        const int _slimExposureDelay(QStringList&);
        const int _slimFocus(QStringList&);
        const int _slimTie(QStringList&);
        const int _slimRepeat(QStringList&);

        const int _slimMax(QStringList&);

        const int _slimSteps(QStringList&);
        const int _slimMotor(QStringList&);
        const int _slimDir(QStringList&);
        const int _slimSpeed(QStringList&);
        const int _slimMs(QStringList&);
        const int _slimMove(QStringList&);
        const int _slimHome(QStringList&);
        const int _slimSetHome(QStringList&);
        const int _slimPlan(QStringList&);
        const int _slimEasing(QStringList&);

        const int _slimStatus(QStringList&);
        const int _slimAddr(QStringList&);

        const int _slimSleep(QStringList&);
        const int _slimMaster(QStringList&);
        const int _slimContinuous(QStringList&);
        const int _slimStopMotor(QStringList&);

        static const char  progNoop   = 0;
        static const char  progPlay   = 1;
        static const char  progPause = 2;
        static const char  progStop   = 3;
        static const char  progCamOn   = 4;
        static const char  progCamOff   = 5;
        static const char  progMove     = 6;
        static const char  progHome     = 7;
        static const char  progShoot    = 8;
        static const char  progKill     = 9;
        static const char  progMaxRate   = 10;
        static const char  progMaxSpeed   = 11;
        static const char  progRamp     = 12;
        static const char  progExpMod   = 13;
        static const char  progAsync    = 14;
        static const char  progDbg      = 15;
        static const char  progStpMot   = 16;
        static const char  progMotMs    = 17;
        static const char  progMast    = 18;
        static const char  progMoveA    = 19;
        static const char  progPlan    = 20;
        static const char  progPlanClr  = 21;
        static const char  progStat     = 100;

        static const char   dataCam     = 1;
        static const char   dataMot     = 2;
        static const char   dataAct     = 3;
        static const char   dataKey     = 4;

        static const char   motSteps    = 1;
        static const char   motRamp     = 2;
        static const char   motDir      = 3;
        static const char   motMax      = 4;
        static const char   motEnable   = 5;
        static const char   motDisable  = 6;
        static const char   motHome     = 7;
        static const char   motBack     = 8;
        static const char   motCont     = 9;
        static const char   motEase     = 10;
        static const char   motContEn   = 11;

        static const char   camInt      = 1;
        static const char   camExp      = 2;
        static const char   camFoc      = 3;
        static const char   camMax      = 4;
        static const char   camPost     = 5;
        static const char   camFWS      = 6;
        static const char   camRpt      = 7;
        static const char   camCAct     = 8;
        static const char   camCAend    = 9;
        static const char   camCMHome   = 10;
        static const char   camCExpDv   = 11;


        static const char    COMPROG = 2;
        static const char    COMDATA = 3;


        static const char statVersion   = 0;
        static const char statRunning   = 1;
        static const char statTime      = 2;
        static const char statCamera    = 3;
        static const char statShots     = 4;
        static const char statInterval  = 5;
        static const char statExp       = 6;
        static const char statPost      = 7;
        static const char statExposing  = 8;
        static const char statMotEn     = 9;
        static const char statMotDir    = 10;
        static const char statMotMoved  = 11;
        static const char statMotHmDist = 12;
        static const char statMotMaxStp = 13;
        static const char statMotRamp   = 14;
        static const char statMotBack   = 15;
        static const char statMotSteps  = 16;


};

#endif // OMAXIS_H
