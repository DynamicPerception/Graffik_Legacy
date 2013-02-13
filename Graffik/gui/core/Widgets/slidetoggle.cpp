#include "slidetoggle.h"

SlideToggle::SlideToggle(QWidget *parent) :
    QSlider(parent)
{
    _setup();
}

SlideToggle::SlideToggle(Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent)
{
    _setup();
}


void SlideToggle::_setup() {
    setMinimum(0);
    setMaximum(1);

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(_getValue(int)));
}

void SlideToggle::_getValue(int p_val) {
    emit selected(p_val);
}
