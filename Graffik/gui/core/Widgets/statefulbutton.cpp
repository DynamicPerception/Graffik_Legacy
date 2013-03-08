#include "statefulbutton.h"

StatefulButton::StatefulButton(QWidget *parent) : QPushButton(parent) {
    m_state = 0;
}

int StatefulButton::state() {
    return m_state;
}

void StatefulButton::setState(int p_state) {
    m_state = p_state;
}
