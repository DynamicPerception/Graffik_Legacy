#ifndef NOTRACKSWIDGET_H
#define NOTRACKSWIDGET_H

#include <QWidget>

namespace Ui {
class NoTracksWidget;
}

class NoTracksWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit NoTracksWidget(QWidget *parent = 0);
    ~NoTracksWidget();
    
private:
    Ui::NoTracksWidget *ui;
};

#endif // NOTRACKSWIDGET_H
