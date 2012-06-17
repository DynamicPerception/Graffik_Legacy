#include "helpwindow.h"
#include "ui_helpwindow.h"

#include <QDebug>

HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HelpWindow)
{
    m_help = new QHelpEngine(QApplication::applicationDirPath() + "/docs/docs.qhc", this);
    m_browse = new HelpBrowser(m_help);

    ui->setupUi(this);

    m_help->setupData();

    if( m_help->error().length() > 0 ) {
        qDebug() << "Error: " << m_help->error();
    }

    QHelpContentWidget* content_w = m_help->contentWidget();

    ui->splitter->addWidget((QWidget*)content_w);
    ui->splitter->addWidget(m_browse);

    QObject::connect((QObject*)content_w, SIGNAL(linkActivated(const QUrl&)), m_browse, SLOT(setSource(const QUrl&)));


}

HelpWindow::~HelpWindow()
{
    delete m_browse;
    delete m_help;
    delete ui;

}

void HelpWindow::on_closeButton_clicked() {
    this->close();
}

