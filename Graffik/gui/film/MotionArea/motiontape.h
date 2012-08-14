#ifndef MOTIONTAPE_H
#define MOTIONTAPE_H

#include <QWidget>

#include "film/FilmParameters/filmparameters.h"

#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QFont>

#define MT_LINE_SPACE   60
#define MT_END_GAP      13
#define MT_LINE_MINSPC  5
#define MT_FONT_SIZE    9

#define MT_LABEL_SS "SS"
#define MT_LABEL_MM "MM"
#define MT_LABEL_HH "HH"
#define MT_LABEL_DD "DD"

#define MT_LABEL_FONT   "courier"

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
    QFont* m_font;

    bool m_drawn;
    unsigned long m_time;
    int m_width;

    void _drawTime(QRect p_rect);
    void _drawLines(QRect p_rect, int p_lines, int p_height, int p_fill = 0, int p_pad = 0);
    float _calcSpacing(QRect p_rect, int p_lines, int p_fill = 0, int p_pad = 0);

};

#endif // MOTIONTAPE_H
