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

#ifndef OMAXISFILMOPTIONS_H
#define OMAXISFILMOPTIONS_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QMutex>
#include <QHash>
#include <QList>

#include "Devices/nanoMoCo/omaxis.h"
#include "MoCoBus/omnetwork.h"
#include "optiontypes.h"

const int OM_OPT_ABSMAXSTEPS = 5000;
const int OM_OPT_VX1MAXSTEPS = 1500;
const int     OM_OPT_VX1REVS = 200;
const float  OM_OPT_VX1RATIO = 10.0;
const int OM_OPT_SX1MAXSTEPS = 1500;
const int     OM_OPT_SX1REVS = 200;
const float  OM_OPT_SX1RATIO = 0.169947;
const float   OM_OPT_JOGDAMP = 5.0;



enum {
    AXIS_MOVE_ROT, AXIS_MOVE_LIN
};

enum {
    AXIS_VX1_PAN, AXIS_VX1_TILT, AXIS_SX1_SLIDE, AXIS_CUSTOM
};


const QString ERR_STR_NFI = "There is no free interval time for interleaved motion, increase interval time or decrease time taken by camera activity";
const QString ERR_STR_NET = "There is not enough total free time to complete the interleaved motion, increase real time of film or decrease distance moved";
const QString ERR_STR_NIT = "One or more intervals have a movement which will cause the interval to be exceeded, increase interval or decrease distance moved.";
const QString ERR_STR_SPD = "Required Move Speed Exceeds Maximum Speed or Step Rate for Axis. Increase Real Time or Decrease Travel Distance.";

    // properties related to film functionality
    // for an axis

struct OMaxisOptions {
    unsigned int axisType;
    bool master;
    float ratio;
    unsigned int maxSteps;
    unsigned int axisMove;
    unsigned int jogLimit;
    float jogDamp;
    unsigned int ms;
    unsigned char backlash;
    float driveRatio;
    unsigned int stepRev;

        // default construction
    OMaxisOptions() {
      //  axisType = AXIS_CUSTOM;
          axisType = AXIS_VX1_PAN;
            master = false;
             ratio = OM_OPT_VX1REVS * OM_OPT_VX1RATIO;
          maxSteps = OM_OPT_VX1MAXSTEPS;
          axisMove = AXIS_MOVE_ROT;
          jogLimit = OM_OPT_VX1MAXSTEPS;
           jogDamp = OM_OPT_JOGDAMP;
                ms = 1;
          backlash = 0;
        driveRatio = OM_OPT_VX1RATIO;
           stepRev = OM_OPT_VX1REVS;

    }

        // construct new from old
    OMaxisOptions(const OMaxisOptions& opts) {
          axisType = opts.axisType;
            master = opts.master;
             ratio = opts.ratio;
          maxSteps = opts.maxSteps;
          axisMove = opts.axisMove;
          jogLimit = opts.jogLimit;
           jogDamp = opts.jogDamp;
                ms = opts.ms;
          backlash = opts.backlash;
        driveRatio = opts.driveRatio;
           stepRev = opts.stepRev;
    }
};

    // we need to be able to serialize OMaxisOptions for things like
    // UserData storing of the data, so let's go ahead and define
    // the needed operators
Q_DECLARE_METATYPE(OMaxisOptions)

inline QDataStream& operator<<(QDataStream& out, const OMaxisOptions& options)
{
    out << quint32(options.axisType);
    out << quint32(options.axisMove);
    out << quint32(options.master);
    out << quint32(options.maxSteps);
    out << QString::number(options.ratio);
    out << quint32(options.jogLimit);
    out << QString::number(options.jogDamp);
    out << quint32(options.ms);
    out << quint8(options.backlash);
    out << QString::number(options.driveRatio);
    out << quint32(options.stepRev);


    return out;
}

inline QDataStream& operator>>(QDataStream& in, OMaxisOptions& options)
{
    quint32 type;
    quint32 move;
    quint32 master;
    quint32 maxSteps;
    QString ratio;
    quint32 jogLimit;
    QString jogDamp;
    quint32 ms;
    quint8  backlash;
    QString driveRatio;
    quint32 stepRev;

    in >> type;
    in >> move;
    in >> master;
    in >> maxSteps;
    in >> ratio;
    in >> jogLimit;
    in >> jogDamp;
    in >> ms;
    in >> backlash;
    in >> driveRatio;
    in >> stepRev;

      options.axisMove = move;
      options.axisType = type;
        options.master = master;
      options.maxSteps = maxSteps;
         options.ratio = ratio.toFloat();
      options.jogLimit = jogLimit;
       options.jogDamp = jogDamp.toFloat();
            options.ms = ms;
      options.backlash = backlash;
    options.driveRatio = driveRatio.toFloat();
       options.stepRev = stepRev;

    return in;
}

/** Axis Options Class

  This class provides containers for, and controls access to, the options for
  motion axes.

  All set, get, and error methods are thread-safe.

  @author
  C. A. Church
  */
class AxisOptions : public QObject
{
    Q_OBJECT
public:
    explicit AxisOptions(QObject *parent = 0);
    ~AxisOptions();
    
    OMaxisOptions* getOptions(unsigned short p_addr);

    void setOptions(unsigned short p_addr, OMaxisOptions* p_opts);
    void setMaster(unsigned short p_addr);

    QList<QString> errors(unsigned short p_addr);
    void clearErrors(unsigned short p_addr);
    void error(unsigned short p_addr, int p_err);
    void removeError(unsigned short p_addr, int p_err);

signals:
    void deviceOptionsChanged(OMaxisOptions* p_opts, unsigned short p_addr);
    void deviceOptionsRemoved(unsigned short p_addr);

public slots:
    void deviceAdded(OMbusInfo* p_bus, OMdeviceInfo* p_dev);
    void deviceDeleted(OMbusInfo* p_bus, unsigned short p_addr);

private:
    QHash<unsigned short, OMaxisOptions*> m_optList;
    QHash<unsigned short, QList<int> > m_errList;
    QMutex* m_optMutex;

    QString _errToString(int p_err);
};

#endif // OMAXISFILMOPTIONS_H
