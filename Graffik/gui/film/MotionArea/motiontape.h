#ifndef MOTIONTAPE_H
#define MOTIONTAPE_H

#include <QWidget>

#include "film/FilmParameters/filmparameters.h"

#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>

namespace Ui {
class MotionTape;
}

class MotionTape : public QWidget
{
    Q_OBJECT
    
public:
    explicit MotionTape(FilmParameters* c_film, QWidget *parent = 0);
    ~MotionTape();

    void paintEvent(QPaintEvent* p_event);
    
public slots:

    void filmUpdated();

private:
    Ui::MotionTape* ui;
    QPainterPath* m_path;
    FilmParameters* m_film;

    bool m_drawn;
    unsigned long m_time;

    void _drawLines(QRect p_rect);

};

#endif // MOTIONTAPE_H
