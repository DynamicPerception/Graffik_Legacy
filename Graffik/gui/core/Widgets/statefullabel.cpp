#include "statefullabel.h"

StatefulLabel::StatefulLabel(QWidget *parent) :  QLabel(parent) {
    m_state = 0;
}

int StatefulLabel::state() {
    return m_state;
}

void StatefulLabel::setState(int p_state) {
    m_state = p_state;
}
