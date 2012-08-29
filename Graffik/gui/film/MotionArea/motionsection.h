#ifndef MOTIONSECTION_H
#define MOTIONSECTION_H

#include <QWidget>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPainter>

#include "film/FilmParameters/filmparameters.h"
#include "film/FilmExec/filmexec.h"


/**

  The MotionSection class creates a transparent section which is displayed
  over the motion path elements, and draws a vertical line where the current
  time is.

  */

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
    void setBorders(int p_leftX, int p_rightX);

private:
   QPainterPath* m_path;
   FilmParameters* m_film;
   FilmExec* m_exec;
   QWidget* m_parent;

   int m_width;
   int m_height;
   int m_scrollWidth;
   unsigned long m_curPos;
   unsigned long m_wasPos;
   unsigned long m_length;
   int m_leftX;
   int m_rightX;

   void _updatePath();

};

#endif // MOTIONSECTION_H
