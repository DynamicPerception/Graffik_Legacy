#ifndef MOTIONSECTION_H
#define MOTIONSECTION_H

#include <QWidget>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPainter>

#include "film/FilmParameters/filmparameters.h"
#include "film/FilmExec/filmexec.h"

#define MS_PRE_GAP  63

class MotionSection : public QWidget
{
    Q_OBJECT
public:
    MotionSection(FilmExec* c_exec, FilmParameters* c_film, QWidget *parent = 0);
    ~MotionSection();

    void paintEvent(QPaintEvent* p_event);

signals:
    

public slots:
    void paramsChanged();
    void playStatusChanged(bool p_stat, unsigned long p_runTime);

private:
   QPainterPath* m_path;
   FilmParameters* m_film;
   FilmExec* m_exec;

   int m_width;
   int m_height;
   unsigned long m_curPos;
   unsigned long m_wasPos;
   unsigned long m_length;

   void _updatePath();

};

#endif // MOTIONSECTION_H
