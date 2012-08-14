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

    int marks = 0;
    int offset = 0;
    int pad = 0;
    int height = 1;

        // We go by largest time set as major marks,
        // with next largest time set as minor marks

    if( days > 0 ) {
        offset = _calcSpacing(p_rect, days);
        offset = (offset / MT_LINE_MINSPC);
        offset = offset > 24 ? 24 : offset;
        marks = days;
            // note that we'll likely have remainding time outside of
            // the
        pad = (float) offset * ((float)(hours - (days * 24)) / 24);
    }

    else if( hours > 0 ) {
        offset = _calcSpacing(p_rect, hours);
        offset = (offset / MT_LINE_MINSPC);
        offset = offset > 60 ? 60 : offset;
        marks = hours;
        pad = (float) offset * ((float)(mins - (hours * 60)) / 60);
    }

    else if( mins > 0  ) {
        offset = _calcSpacing(p_rect, mins);
        offset = (offset / MT_LINE_MINSPC);
        offset = offset > 60 ? 60 : offset;
        marks = mins;
        pad = (float) offset * ((float)(secs - (mins * 60)) / 60);
    }
    else {
        offset = _calcSpacing(p_rect, secs);
        offset = (offset / MT_LINE_MINSPC);
        offset = offset > 100 ? 100 : offset;
        marks = secs;
        pad = (float) offset * ((float)(m_time  - (secs * 1000)) / 100);
    }

    _drawLines(p_rect, marks, height, offset, pad);

}

void MotionTape::_drawLines(QRect p_rect, int p_lines, int p_height, int p_fill, int p_pad) {

    float pixStep = _calcSpacing(p_rect, p_lines, p_fill, p_pad);
    int wholeStep = int(pixStep);
    int curPx = MT_LINE_SPACE; // pad to beginning of actual timeline display
    float err = 0.0;
    float fillStep = 0.0;
    int fillWhole = 0;

        // if we have fill lines...
    if( p_fill > 0 )
        fillStep = pixStep / (float) ( p_fill + 1 );

    pixStep -= int(pixStep);

    fillWhole = int(fillStep);
    fillStep -= int(fillStep);

    for( unsigned long i = 0; i < p_lines; i++) {

        unsigned long fillPos = curPx + fillWhole;
        float fillErr = 0.0;

            // major time marks indicate that much time has passed,
            // so we start by filling minor marks, and then move to
            // major marks
        for( int f = 0; f < p_fill; f++) {

            m_path->moveTo(fillPos, p_rect.height());
            m_path->lineTo(fillPos, p_rect.height() - (p_rect.height() / (p_height * 4)) );
            fillPos += fillWhole;
            fillErr += fillStep;
            if( fillErr > 0 ) {
                fillPos += 1;
                fillErr -= 1.0;
            }
        }

        curPx += wholeStep;
        err += pixStep;

        if( err > 0.0 ) {
            curPx++;
            err -= 1.0;
        }

        m_path->moveTo(curPx, p_rect.height());
        m_path->lineTo(curPx, p_rect.height() - (p_rect.height() / p_height) );


    }

        // if there was remainder time left over, draw it out
    if( p_pad > 0 ) {
        unsigned long fillPos = curPx + fillWhole;
        float fillErr = 0.0;

        for( int f = 0; f < p_pad; f++) {

          m_path->moveTo(fillPos, p_rect.height());
          m_path->lineTo(fillPos, p_rect.height() - (p_rect.height() / (p_height * 4)) );
          fillPos += fillWhole;
          fillErr += fillStep;
          if( fillErr > 0 ) {
             fillPos += 1;
             fillErr -= 1.0;
          }
        }
    }
}

float MotionTape::_calcSpacing(QRect p_rect, int p_lines, int p_fill, int p_pad) {

    if( p_pad > 0 ) {
        float realLines = (float) p_lines + ((float)p_pad / (float)p_fill);
        return ((float) p_rect.width() - (float) MT_LINE_SPACE) / realLines;
    }
    return ((float) p_rect.width() - (float) MT_LINE_SPACE) / (float) p_lines;
}