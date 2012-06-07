#include "helpbrowser.h"

HelpBrowser::HelpBrowser(QHelpEngine *c_help, QWidget *parent) :
    QTextBrowser(parent)
{
    m_help = c_help;
}

QVariant HelpBrowser::loadResource(int type, const QUrl &url) {
    if (url.scheme() == "qthelp")
        return QVariant(m_help->fileData(url));
    else
        return QTextBrowser::loadResource(type, url);
}
