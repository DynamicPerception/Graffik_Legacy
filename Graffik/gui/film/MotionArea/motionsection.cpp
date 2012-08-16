#include "motionsection.h"

#include <QDebug>

MotionSection::MotionSection(FilmExec *c_exec, FilmParameters *c_film, QWidget *parent) :
    QWidget(parent)
{
    m_path = new QPainterPath;
    m_film = c_film;
    m_exec = c_exec;

    m_width  = 0;
    m_height = 0;
    m_curPos = 0;
    m_wasPos = 0;
    m_length = 0;

        // we're making a transparent widget, to overlay the section
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(paramsChanged()));
}

MotionSection::~MotionSection() {
    delete m_path;
}

void MotionSection::paintEvent(QPaintEvent * p_event) {
    qDebug() << "MS: pE";

    QPainter painter(this);

    if( p_event->rect().width() != m_width ||
            p_event->rect().height() != m_height ||
            m_curPos != m_wasPos ) {

        m_width  = p_event->rect().width();
        m_height = p_event->rect().height();
        m_wasPos = m_curPos;

        qDebug() << "PE: WH:" << m_width << m_height;
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

    unsigned int curPos = ( (float) m_length / (float) (m_width - MS_PRE_GAP)) * ((float) m_curPos / (float) m_length);

    m_path->moveTo(MS_PRE_GAP + curPos, 0);
    m_path->lineTo(MS_PRE_GAP + curPos, m_height);
}



