#include "motionsection.h"

#include <QDebug>
#include <QLayout>

MotionSection::MotionSection(FilmExec *c_exec, FilmParameters *c_film, QWidget *parent) :
    QWidget(parent)
{
    m_path = new QPainterPath;
    m_film = c_film;
    m_exec = c_exec;
    m_parent = parent;

    m_width  = 0;
    m_height = 0;
    m_curPos = 0;
    m_wasPos = 0;
    m_length = 0;

    m_scrollWidth = m_parent->width() - m_parent->layout()->contentsMargins().right();

        // we're making a transparent widget, to overlay the section and draw over all children
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(paramsChanged()));
    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), this, SLOT(playStatusChanged(bool,ulong)));

}

MotionSection::~MotionSection() {
    delete m_path;
}

void MotionSection::paintEvent(QPaintEvent * p_event) {
     QPainter painter(this);

    if( p_event->rect().width() != m_width ||
            p_event->rect().height() != m_height ||
            m_curPos != m_wasPos ) {

        m_width  = p_event->rect().width();
        m_height = p_event->rect().height();
        m_wasPos = m_curPos;
        _updatePath();
    }

    painter.drawPath(*m_path);

}


void MotionSection::paramsChanged() {

    OMfilmParams params = m_film->getParamsCopy();

    if( params.realLength == m_length )
        return;


    m_length = params.realLength;
    _updatePath();
    this->update();
}


void MotionSection::playStatusChanged(bool p_stat, unsigned long p_runTime) {

    m_curPos = p_runTime;
    this->update();
}

void MotionSection::_updatePath() {

    delete m_path;
    m_path = new QPainterPath;

    m_scrollWidth = m_parent->width() - m_parent->layout()->contentsMargins().right();
    int diff = m_width - m_scrollWidth;

    int curXpos = ((float) m_curPos / (float) m_length) * (float) (m_width - MS_PRE_GAP - diff);

    m_path->moveTo(MS_PRE_GAP + curXpos, 0);
    m_path->lineTo(MS_PRE_GAP + curXpos, m_height);
}



