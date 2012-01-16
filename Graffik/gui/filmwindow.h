#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>

namespace Ui {
class filmWindow;
}

class filmWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit filmWindow(QWidget *parent = 0);
    ~filmWindow();
    
private:
    Ui::filmWindow *ui;
};

#endif // FILMWINDOW_H
