#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QtHelp/QHelpEngine>
#include <QVariant>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    HelpBrowser(QHelpEngine* c_help, QWidget *parent = 0);
    
    QVariant loadResource(int type, const QUrl &url);

signals:
    
public slots:
    
private:
   QHelpEngine* m_help;
};

#endif // HELPBROWSER_H
