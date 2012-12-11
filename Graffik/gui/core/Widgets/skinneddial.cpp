#include "skinneddial.h"

#include <QPaintEvent>
#include <QTransform>

#include <QDebug>

SkinnedDial::SkinnedDial(QWidget *parent) : QDial(parent) {

    m_background = new QPixmap;
    m_needle     = new QPixmap;
    m_maxDeg     = 180.0;
    m_paintFlags = QPainter::RenderHint(QPainter::Antialiasing | QPainter::SmoothPixmapTransform || QPainter::HighQualityAntialiasing);

}

SkinnedDial::SkinnedDial(QPixmap* c_back, QPixmap* c_needle, float c_angle, QWidget *parent) : QDial(parent) {
    m_background = c_back;
    m_needle     = c_needle;
    m_maxDeg     = c_angle;
    m_paintFlags = QPainter::RenderHint(QPainter::Antialiasing | QPainter::SmoothPixmapTransform || QPainter::HighQualityAntialiasing);

}

SkinnedDial::~SkinnedDial() {
    delete m_background;
    delete m_needle;
}

void SkinnedDial::setBackgroundImage(QPixmap p_img) {
    *m_background = p_img;
}

QPixmap SkinnedDial::backgroundImage() {
    return *m_background;
}

void SkinnedDial::setNeedleImage(QPixmap p_img) {
    *m_needle = p_img;
}

QPixmap SkinnedDial::needleImage() {
    return *m_needle;
}

float SkinnedDial::maxAngle() {
    return m_maxDeg;
}

void SkinnedDial::setMaxAngle(float p_angle) {
    m_maxDeg = p_angle;
}

void SkinnedDial::paintEvent(QPaintEvent *pe) {

    QPainter painter(this);
    QRect eventRect = pe->rect();


        // scale pixmap so that it fits within the
        // boundaries of the event

    int        height = eventRect.height();
    int         width = eventRect.width();
    QPixmap    newPix = *m_background;
    QPixmap newNeedle = *m_needle;
    QPixmap rotNeedle;

    if( width >= height ) {
        if( height <= m_background->height() ) {
            newPix = m_background->scaledToHeight(height, Qt::SmoothTransformation);
            newNeedle = m_needle->scaledToHeight(height, Qt::SmoothTransformation);
        }
    }
    else {
        if( width <= m_background->width() ) {
            newPix = m_background->scaledToWidth(width, Qt::SmoothTransformation);
            newNeedle = m_needle->scaledToWidth(width, Qt::SmoothTransformation);
        }
    }



     // find top-left corner to start placing the pixmap for the background,
     // centering it in the total event area

    int leftPoint = (width - newPix.width()) / 2;
    int topPoint  = (height - newPix.height()) / 2;

    QPoint startPix(leftPoint, topPoint);

    painter.setRenderHints(m_paintFlags);
    painter.drawPixmap(startPix, newPix);

        // rotate the needle image and display it

    int curVal = this->value();

    if( curVal == 0 ) {
        painter.drawPixmap(startPix, newNeedle);
    }
    else if( curVal < 0 ) {
        float pct = (float) abs(curVal) / (float) abs(this->minimum());
        rotNeedle = _rotatePix(&newNeedle, m_maxDeg * pct, false);
    }
    else {
        float pct =(float)  abs(curVal) / (float) abs(this->maximum());
        rotNeedle = _rotatePix(&newNeedle, m_maxDeg * pct, true);
    }

    painter.drawPixmap(startPix, rotNeedle);

}

QPixmap SkinnedDial::_rotatePix(QPixmap *p_pix, float p_deg, bool p_dir) {
    qDebug() << "SD: Rotate:" << m_maxDeg << p_deg << p_dir;

    if( p_dir == false )
        p_deg = 360.0 - p_deg;

    QTransform trans;
    QTransform transRot = trans.rotate(p_deg);

    QPixmap outPix = p_pix->transformed(transRot);

        // re-crop to original size
    int xoffset = (outPix.width() - p_pix->width()) / 2;
    int yoffset = (outPix.height() - p_pix->height()) / 2;
    outPix = outPix.copy(xoffset, yoffset, p_pix->width(), p_pix->height());

    return outPix;
}
