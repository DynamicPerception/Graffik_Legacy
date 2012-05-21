#ifndef OMAXISFILMOPTIONS_H
#define OMAXISFILMOPTIONS_H

#include <QObject>
#include <QString>

#include "Devices/nanoMoCo/omaxis.h"
#include "MoCoBus/omnetwork.h"

#define OM_OPT_ABSMAXSTEPS    5000
#define OM_OPT_VX1MAXSTEPS    2500
#define OM_OPT_VX1RATIO       90.0

enum {
    AXIS_VX1_PAN, AXIS_VX1_TILT, AXIS_SX1_SLIDE
};

struct OMaxisOptions {
    unsigned int axisType;
    bool master;
    float ratio;
    unsigned int maxSteps;

        // default construction
    OMaxisOptions() {
        axisType = AXIS_VX1_PAN;
        master = false;
        ratio = OM_OPT_VX1RATIO;
        maxSteps = OM_OPT_VX1MAXSTEPS;
    }
};


class OMAxisFilmOptions : public QObject
{
    Q_OBJECT
public:
    explicit OMAxisFilmOptions(QObject *parent = 0);
    ~OMAxisFilmOptions();
    
    OMaxisOptions* getOptions(unsigned short p_addr);

    void setOptions(unsigned short p_addr, OMaxisOptions* p_opts);

signals:
    
public slots:
    void deviceAdded(OMbusInfo* p_bus, OMdeviceInfo* p_dev);


private:
    QHash<unsigned short, OMaxisOptions*> m_optList;


};

#endif // OMAXISFILMOPTIONS_H
