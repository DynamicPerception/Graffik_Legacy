#ifndef FILMPARAMETERS_H
#define FILMPARAMETERS_H

#include <QHash>
#include <QWidget>
#include <QString>
#include <QMutex>

#include "MoCoBus/omnetwork.h"

 // some defaults and maximum values

#define CAM_DEF_EXPOSURE    200
#define CAM_DEF_DELAY       1000
#define CAM_DEF_FOCUS       0
#define CAM_DEF_INTERVAL    1

#define CAM_MAX_EXPOSURE    4294967296
#define CAM_MAX_DELAY       4294967296
#define CAM_MAX_FOCUS       4294967296

 /** Axis Easing Type */

enum {
    AXIS_CURVE_LIN, AXIS_CURVE_QUAD, AXIS_CURVE_INVQUAD
};

 /** Film Motion type */

enum {
    FILM_MODE_CONT, FILM_MODE_SMS
};

/** Camera Parameters for a Specific Film */
struct OMfilmCamParams {
        /** Camera control enabled */
    bool camControl;
        /** Focus lock with Shutter */
    bool focLock;
        /** Shutter Time */
    unsigned long shutterMS;
        /** Exposure Delay */
    unsigned long delayMS;
        /** Focus Time */
    unsigned long focusMS;
        /** Bulb exposure control */
    bool bulb;
        /** Focus enabled */
    bool focus;
        /** Manual Interval Control */
    bool manInterval;
        /** Interval time (seconds) */
    unsigned long interval;
        /** Auto-calculate FPS */
    bool autoFPS;

        /** Default Constructor */
    OMfilmCamParams() {
        camControl = false;
        focLock = true;
        shutterMS = CAM_DEF_EXPOSURE;
        delayMS = CAM_DEF_DELAY;
        focusMS = CAM_DEF_FOCUS;
        bulb = false;
        focus = false;
        manInterval = false;
        interval = CAM_DEF_INTERVAL;
        autoFPS = true;
    }

};

 /** Film Parameters for a Specific Axis */
struct OMfilmAxisParams {
        /** Bus of device */
    QString bus;
        /** Ending distance for movement */
    unsigned long endDist;
        /** Type of easing */
    int easing;
        /** Acceleration time (ms) */
    unsigned long accelTm;
        /** Deceleration time (ms) */
    unsigned long decelTm;
        /** Microstep setting for axis */
    unsigned int ms;
        /** Start Running Time */
    unsigned long startTm;
        /** end Running Time */
    unsigned long endTm;

    OMfilmAxisParams() {
        bus = "";
        endDist = 0;
        easing = AXIS_CURVE_QUAD;
        accelTm = 0;
        decelTm = 0;
        ms = 1;
        startTm = 0;
        endTm = 0;
    }

};

 /** Parameters for the Film Program */
struct OMfilmParams {
        /** Film length (output film) */
    unsigned long length;
        /** Film execution (wall clock) time */
    unsigned long realLength;
        /** Pointer to camera parameters */
    OMfilmCamParams* camParams;
        /** A hash of parameters for each axis */
    QHash<unsigned short, OMfilmAxisParams*> axes;
        /** Film Motion Type */
    int filmMode;
        /** Frames per second */
    unsigned short fps;

    OMfilmParams() {
        length = 1000 * 60 * 42;
        realLength = 1000 * 60 * 42;
        camParams = new OMfilmCamParams;
        fps = 24;
    }
};


/** Film Parameters

  This class manages the parameters for any given film.  It provides
  methods to access and set the film parameters, including those for
  each axis.  All accessor/settor methods are thread-safe.

  @author C. A. Church
  Copyright &copy; 2012 Dynamic Perception LLC
  This software is licensed under the GPLv3 Software License
  */

class FilmParameters : public QWidget
{
    Q_OBJECT
public:

    explicit FilmParameters(OMNetwork* c_net, QWidget* parent = 0);
    ~FilmParameters();

    OMfilmParams* getParams();
    OMfilmParams getParamsCopy();

    void releaseParams();

private slots:

    void _addDevice(OMbusInfo *p_bus, OMdeviceInfo *p_dev);
    void _removeDevice(QString p_bus, unsigned short p_addr);

public slots:

private:

    OMNetwork* m_net;
    OMfilmParams* m_filmParams;
    QMutex* m_mutex;
    bool m_isLocked;
};

#endif // FILMPARAMETERS_H
