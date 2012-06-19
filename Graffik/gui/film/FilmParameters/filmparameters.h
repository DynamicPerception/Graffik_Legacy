#ifndef FILMPARAMETERS_H
#define FILMPARAMETERS_H

#include <QHash>
#include <QWidget>
#include <QString>

#include "MoCoBus/omnetwork.h"

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

        /** Default Constructor */
    OMfilmCamParams() {
        camControl = false;
        focLock = true;
        shutterMS = 100;
        delayMS = 0;
        focusMS = 0;
    }

};

 /** Film Parameters for a Specific Axis */
struct OMfilmAxisParams {
        /** Ending position for movement */
    unsigned long endPos;
        /** Type of easing */
    int easing;
        /** Acceleration time (ms) */
    unsigned long accelTm;
        /** Deceleration time (ms) */
    unsigned long decelTm;
        /** Microstep setting for axis */
    unsigned int ms;

    OMfilmAxisParams() {
        endPos = 0;
        easing = AXIS_CURVE_QUAD;
        accelTm = 0;
        decelTm = 0;
        ms = 1;
    }

};

 /** Parameters for the Film Program */
struct OMfilmParams {
        /** Film (output) length */
    unsigned long length;
        /** Film execution (wall clock) time */
    unsigned long realLength;
        /** Pointer to camera parameters */
    OMfilmCamParams* camParams;
        /** A hash of parameters for each axis */
    QHash<unsigned short, OMfilmAxisParams*> axes;
        /** Film Motion Type */
    int filmMode;

    OMfilmParams() {
        length = 1000 * 60 * 42;
        realLength = 1000 * 60 * 42;
        camParams = new OMfilmCamParams;
    }
};

class FilmParameters : public QWidget
{
    Q_OBJECT
public:

    explicit FilmParameters(OMNetwork* c_net, QWidget* parent = 0);
 //   ~FilmParameters();


private slots:

    void _addDevice(OMdeviceInfo* p_dev);
    void _removeDevice(QString p_bus, unsigned short p_addr);

public slots:

private:

    OMNetwork* m_net;
    OMfilmParams* m_filmParams;
};

#endif // FILMPARAMETERS_H
