#include "skinneddial.h"

#include <QPaintEvent>
#include <QPainter>

SkinnedDial::SkinnedDial(QWidget *parent) :
    QDial(parent)
{

    m_background = new QPixmap;
    m_needle     = new QPixmap;
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

void SkinnedDial::paintEvent(QPaintEvent *pe) {
    QPainter painter(this);
    QRect eventRect = pe->rect();

    QPainter::RenderHints paintFlags(QPainter::Antialiasing | QPainter::SmoothPixmapTransform || QPainter::HighQualityAntialiasing);
//    paintFlags = QPainter::Antialiasing | QPainter::SmoothPixmapTransform || QPainter::HighQualityAntialiasing;

    int height = eventRect.height();
    QPixmap newPix = m_background->scaledToHeight(height, Qt::SmoothTransformation);

    painter.setRenderHints(paintFlags);
    painter.drawPixmap(eventRect.topLeft(), newPix);

}
