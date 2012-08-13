#ifndef MOTIONTAPE_H
#define MOTIONTAPE_H

#include <QWidget>

#include "film/FilmParameters/filmparameters.h"

#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>

#define MT_LINE_SPACE   50

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
    int m_width;

    void _drawTime(QRect p_rect);
    void _drawLines(QRect p_rect, int p_lines, int p_height, int p_offset = 0);

};

#endif // MOTIONTAPE_H
