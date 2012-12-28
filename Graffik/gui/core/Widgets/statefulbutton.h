#ifndef STATEFULBUTTON_H
#define STATEFULBUTTON_H

#include <QPushButton>

/** Stateful Button

  A QPushButton with a configurable state that can be accessed via CSS.

  */

class StatefulButton : public QPushButton
{
    Q_OBJECT
    /** State Property

      Set a state property that can be read/written via CSS.  Default value
      is 0.
      */
    Q_PROPERTY(int state READ state WRITE setState DESIGNABLE true)
public:
    explicit StatefulButton(QWidget *parent = 0);
    
    int state();
    void setState(int p_state);

signals:
    
public slots:
    
private:
    int m_state;
};

#endif // STATEFULBUTTON_H
