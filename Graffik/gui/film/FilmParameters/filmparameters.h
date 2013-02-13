/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

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

#ifndef FILMPARAMETERS_H
#define FILMPARAMETERS_H

#include <QHash>
#include <QWidget>
#include <QString>
#include <QMutex>
#include <QDataStream>
#include <QDebug>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

 // some defaults and maximum values

const int CAM_DEF_EXPOSURE =   200;
const int CAM_DEF_DELAY    =   1000;
const int CAM_DEF_FOCUS    =   0;
const int CAM_DEF_INTERVAL =  1000;

const unsigned long CAM_MAX_EXPOSURE =   ULONG_MAX;
const unsigned long CAM_MAX_DELAY    =   ULONG_MAX;
const unsigned long CAM_MAX_FOCUS    =   ULONG_MAX;

 // change when you change structure of film parameters
 // to prevent crashes and other unhappiness when attempting
 // to load a saved film from an old version

const int OM_FILM_FMT_VER   =   1;

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
        /** Interval time (milliseconds) */
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


// Provide streaming operators for use in persistance, writing to files, etc.

Q_DECLARE_METATYPE(OMfilmCamParams)

inline QDataStream& operator<<(QDataStream& out, const OMfilmCamParams& params) {
    out << params.camControl;
    out << params.focLock;
    out << quint32(params.shutterMS);
    out << quint32(params.delayMS);
    out << quint32(params.focusMS);
    out << params.bulb;
    out << params.focus;
    out << params.manInterval;
    out << quint32(params.interval);
    out << params.autoFPS;

    return out;
}

inline QDataStream& operator>>(QDataStream& in, OMfilmCamParams& options) {

    quint32 shutterMS;
    quint32 delayMS;
    quint32 focusMS;
    quint32 interval;

    in >> options.camControl;
    in >> options.focLock;
    in >> shutterMS;
        options.shutterMS = shutterMS;
    in >> delayMS;
        options.delayMS = delayMS;
    in >> focusMS;
        options.focusMS = focusMS;
    in >> options.bulb;
    in >> options.focus;
    in >> options.manInterval;
    in >> interval;
        options.interval = interval;
    in >> options.autoFPS;

    return in;
}

 /** Film Parameters for a Specific Axis */
struct OMfilmAxisParams {
        /** Bus of device */
    QString bus;
        /** Ending distance for movement + direction */
    long endDist;
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
        /** Mute Axis (don't run) */
    bool mute;

    OMfilmAxisParams() {
        bus = "";
        endDist = 0;
        easing = OM_MOT_QUAD;
        accelTm = 0;
        decelTm = 0;
        ms = 1;
        startTm = 0;
        endTm = 0;
        mute = false;
    }

};


Q_DECLARE_METATYPE(OMfilmAxisParams)

inline QDataStream& operator<<(QDataStream& out, const OMfilmAxisParams& params) {
    out << params.bus;
    out << qint32(params.endDist);
    out << qint16(params.easing);
    out << quint32(params.accelTm);
    out << quint32(params.decelTm);
    out << qint16(params.ms);
    out << quint32(params.startTm);
    out << quint32(params.endTm);
    out << params.mute;

    return out;
}

inline QDataStream& operator>>(QDataStream& in, OMfilmAxisParams& options) {


    qint32 endDist;
    qint16 easing;
    quint32 accelTm;
    quint32 decelTm;
    qint16 ms;
    quint32 startTm;
    quint32 endTm;

    in >> options.bus;
    in >> endDist;
        options.endDist = endDist;
    in >> easing;
        options.easing = easing;
    in >> accelTm;
        options.accelTm = accelTm;
    in >> decelTm;
        options.decelTm = decelTm;
    in >> ms;
        options.ms = ms;
    in >> startTm;
        options.startTm = startTm;
    in >> endTm;
        options.endTm = endTm;
    in >> options.mute;

    return in;
}


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
        length = 1000 * 60 * 2;
        realLength = 1000 * 60 * 2;
        camParams = new OMfilmCamParams;
        filmMode = FILM_MODE_CONT;
        fps = 24;
    }
};

Q_DECLARE_METATYPE(OMfilmParams)

inline QDataStream& operator<<(QDataStream& out, const OMfilmParams& params) {
    out << quint32(params.length);
    out << quint32(params.realLength);
    out << *params.camParams;
    out << quint32(params.filmMode);
    out << quint32(params.fps);

    quint32 axCnt = params.axes.count();

    out << axCnt;

    foreach(unsigned short addr, params.axes.keys()) {
        out << quint32(addr);
        OMfilmAxisParams* axis = params.axes.value(addr);
        out << *axis;
    }

    return out;
}

inline QDataStream& operator>>(QDataStream& in, OMfilmParams& params) {

   quint32 length;
   quint32 realLength;
   OMfilmCamParams camParams;
   quint32 filmMode;
   quint32 fps;
   quint32 axCnt;

   in >> length;
    params.length = length;
   in >> realLength;
    params.realLength = realLength;
   in >> camParams;
    params.camParams = new OMfilmCamParams(camParams);
   in >> filmMode;
    params.filmMode = filmMode;
   in >> fps;
    params.fps = fps;
   in >> axCnt;

   for(quint32 i = 0; i < axCnt; i++) {
       quint32 addr;
       in >> addr;
       OMfilmAxisParams axis;
       in >> axis;
       params.axes.insert(addr, new OMfilmAxisParams(axis));
   }


    return in;
}


/** Film Parameters

  This class manages the parameters for any given film.  It provides
  methods to access and set the film parameters, including those for
  each axis.  All accessor/settor methods are thread-safe.

  @author C. A. Church
  Copyright &copy; 2012 Dynamic Perception LLC
  */

class FilmParameters : public QWidget
{
    Q_OBJECT
public:

    explicit FilmParameters(OMNetwork* c_net, QWidget* parent = 0);
    ~FilmParameters();

    OMfilmParams* getParams();
    OMfilmParams getParamsCopy();

    void releaseParams(bool broadcast = true);

private slots:

    void _addDevice(OMbusInfo *p_bus, OMdeviceInfo *p_dev);
    void _removeDevice(QString p_bus, unsigned short p_addr);

public slots:

signals:
    void paramsReleased();

private:

    OMNetwork* m_net;
    OMfilmParams* m_filmParams;
    QMutex* m_mutex;
    bool m_isLocked;
};

#endif // FILMPARAMETERS_H
