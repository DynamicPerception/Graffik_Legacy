#include "flicknumberitem.h"

FlickNumberItem::FlickNumberItem(QString c_first, QString c_second, QWidget *parent) : QWidget(parent) {

   // m_widget = new QWidget;
    m_layout = new QHBoxLayout;
    m_first  = new QLabel(c_first);
    m_second = new QLabel(c_second);

    m_layout->addWidget(m_first);
    m_layout->addWidget(m_second);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setLayout(m_layout);

   // setSizeHint(m_widget->sizeHint());
}

FlickNumberItem::~FlickNumberItem()
{

   // delete m_widget;
    delete m_layout;
    delete m_first;
    delete m_second;
}

/*QWidget* FlickNumberItem::widget() {

    return m_widget;
}*/

void FlickNumberItem::setFirst(QString p_str) {
    m_first->setText(p_str);
}

void FlickNumberItem::setSecond(QString p_str) {
    m_second->setText(p_str);
}
