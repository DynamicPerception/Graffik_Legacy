#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QtHelp/QHelpEngine>
#include "helpbrowser.h"

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();
    
public slots:
    void on_closeButton_clicked();

private:
    Ui::HelpWindow *ui;
    QHelpEngine* m_help;
    HelpBrowser* m_browse;
};

#endif // HELPWINDOW_H
