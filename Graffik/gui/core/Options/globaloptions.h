#ifndef GLOBALOPTIONS_H
#define GLOBALOPTIONS_H

#include <QObject>
#include <QMutex>


#include "optiontypes.h"

/** Global Options Class

  This class manages a set of global options available to the user interface.

  All set and get methods are thread-safe.

  */

class GlobalOptions : public QObject
{
    Q_OBJECT
public:
    explicit GlobalOptions(QObject *parent = 0);
    ~GlobalOptions();

    bool stopOnErr();
    void stopOnErr(bool p_mode);

    int display();
    void display(int p_mode);
    
signals:
    void optionsChanged();

public slots:
    
private:
    bool m_stopErr;
    int m_display;

    QMutex* m_mutex;
};

#endif // GLOBALOPTIONS_H
