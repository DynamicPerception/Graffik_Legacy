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
    }
}

void MotionTape::paintEvent(QPaintEvent *p_event) {
    QPainter painter(this);
    QRect eventRect = p_event->rect();

    qDebug() << "MT: Paint Event";

    if( ! m_drawn ) {
        delete m_path;
        m_path = new QPainterPath;
        _drawLines(eventRect);
        m_drawn = true;
    }

    painter.drawPath(*m_path);

}

void MotionTape::_drawLines(QRect p_rect) {

    qDebug() << "MT: Drawing Time Lines" << m_time;

        // seconds
    unsigned long secs = m_time / 1000;
    unsigned long mins = secs / 60;
    unsigned long hours = mins / 60;
    unsigned long days = hours / 24;

    if( days > 0 ) {
    }

    else if( hours > 0 ) {

    }

    else if( mins > 0  ) {
        int pixMin = p_rect.width() / mins;
        int curPx = 0;

        for( unsigned long i = 1; i < mins; i++) {
            m_path->moveTo(curPx, 0);
            m_path->lineTo(curPx, p_rect.top() );
            curPx += pixMin;
        }
    }
    else {

    }
}
