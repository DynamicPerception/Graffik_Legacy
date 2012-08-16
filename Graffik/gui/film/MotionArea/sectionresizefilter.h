#ifndef SECTIONRESIZEFILTER_H
#define SECTIONRESIZEFILTER_H

#include <QObject>
#include <QWidget>
#include "motionsection.h"

class SectionResizeFilter : public QObject
{
    Q_OBJECT
public:
    explicit SectionResizeFilter(MotionSection* c_motion, QObject *parent = 0);
    
signals:
    
public slots:
    
protected:
    bool eventFilter(QObject *p_obj, QEvent *p_event);

private:
    MotionSection* m_motion;
};

#endif // SECTIONRESIZEFILTER_H
