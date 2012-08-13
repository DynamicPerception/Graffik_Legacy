#include "motiontape.h"
#include "ui_motiontape.h"

#include <QDebug>


MotionTape::MotionTape(FilmParameters *c_film, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionTape)
{

    ui->setupUi(this);

    m_film = c_film;


    m_drawn = false;
    m_time  = 0;
    m_path = new QPainterPath;

    OMfilmParams parms = m_film->getParamsCopy();
    m_time  = parms.realLength;
    m_width = this->width();

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(filmUpdated()));

}

MotionTape::~MotionTape()
{
    delete ui;
    delete m_path;
}

void MotionTape::filmUpdated() {

    qDebug() << "MT: Got Film Update";

    OMfilmParams parms = m_film->getParamsCopy();
    if( parms.realLength != m_time ) {
        m_drawn = false;
        m_time  = parms.realLength;
        this->update();
    }
}

void MotionTape::paintEvent(QPaintEvent *p_event) {
    QPainter painter(this);
    QRect eventRect = p_event->rect();


    qDebug() << "MT: Paint Event";

    if( ! m_drawn || m_width != eventRect.width() ) {
        delete m_path;
        m_width = eventRect.width();
        m_path = new QPainterPath;
        _drawTime(eventRect);
        m_drawn = true;
    }

    painter.drawPath(*m_path);

}

void MotionTape::_drawTime(QRect p_rect) {

    qDebug() << "MT: Drawing Time Lines" << m_time;

        // seconds
    unsigned long secs = m_time / 1000;
    unsigned long mins = secs / 60;
    unsigned long hours = mins / 60;
    unsigned long days = hours / 24;

    int height = 1;
    int offset = 0;

    if( days > 0 ) {
    }

    else if( hours > 0 ) {

    }

    else if( mins > 0  ) {
        _drawLines(p_rect, mins, height, offset);
    }
    else {

    }
}

void MotionTape::_drawLines(QRect p_rect, int p_lines, int p_height, int p_offset) {

    float pixStep = (p_rect.width() - MT_LINE_SPACE - p_offset) / p_lines;

    qDebug() << "MT: Drawing Lines" << p_lines << p_offset << pixStep;

    int curPx = MT_LINE_SPACE + p_offset;
    int wholeStep = int(pixStep);
    float err = 0.0;

    pixStep -= int(pixStep);

    for( unsigned long i = 0; i < p_lines; i++) {

        curPx += wholeStep;
        err += pixStep;

        if( err > 0.0 ) {
            curPx++;
            err -= 1.0;
        }

        m_path->moveTo(curPx, 0);
        m_path->lineTo(curPx, (p_rect.height() / p_height) );

    }

}
