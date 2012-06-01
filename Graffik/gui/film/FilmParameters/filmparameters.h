#ifndef FILMPARAMETERS_H
#define FILMPARAMETERS_H

#include <QHash>

enum {
    AXIS_CURVE_LIN, AXIS_CURVE_QUAD, AXIS_CURVE_INVQUAD
};

enum {
    FILM_MODE_CONT, FILM_MODE_SMS
};

struct OMfilmCamParams {
    bool camControl;
    bool focLock;
    unsigned long shutterMS;
    unsigned long delayMS;
    unsigned long focusMS;

    OMfilmCamParams() {
        camControl = false;
        focLock = true;
        shutterMS = 100;
        delayMS = 0;
        focusMS = 0;
    }

};

struct OMfilmAxisParams {
    unsigned long endPos;
    int curve;
    unsigned long accelTm;
    unsigned long decelTm;

};

struct OMfilmParams {
    unsigned long lengthMS;
    OMfilmCamParams* camParams;
    QHash<unsigned short, OMfilmAxisParams> axes;
    int filmMode;

};

class FilmParameters
{
public:
    FilmParameters();
    ~FilmParameters();

};

#endif // FILMPARAMETERS_H
