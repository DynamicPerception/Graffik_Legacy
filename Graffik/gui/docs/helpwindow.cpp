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

    qDebug() << "Error: " << m_help->error();


    QHelpContentModel* content_m = m_help->contentModel();
    QHelpContentWidget* content_w = m_help->contentWidget();



    ui->splitter->addWidget((QWidget*)content_w);
    ui->splitter->addWidget(m_browse);

    QObject::connect((QObject*)content_w, SIGNAL(linkActivated(const QUrl&)), m_browse, SLOT(setSource(const QUrl&)));

/*    QObject::connect(index_w, SIGNAL(linkActivated(const QUrl &)),
    connect(helpEngine->contentWidget(),
                SIGNAL(linkActivated(const QUrl &)),
                helpBrowser, SLOT(setSource(const QUrl &))); */
    //index_w->setModel(index_m);
    //content_w->setModel(content_m);
}

HelpWindow::~HelpWindow()
{
    delete m_browse;
    delete m_help;
    delete ui;

}

