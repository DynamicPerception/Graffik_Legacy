#ifndef GLOBALOPTIONSDIALOG_H
#define GLOBALOPTIONSDIALOG_H

#include <QDialog>

#include "globaloptions.h"

#define GOD_IMP_LABEL   "Imperial"
#define GOD_MET_LABEL   "Metric"
#define GOD_STP_LABEL   "Steps"

namespace Ui {
    class GlobalOptionsDialog;
}

class GlobalOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    GlobalOptionsDialog(GlobalOptions* c_opts, QWidget *parent = 0);
    ~GlobalOptionsDialog();

public slots:
    void accept();
    
private:
    Ui::GlobalOptionsDialog *ui;

    GlobalOptions* m_opts;

    void _setupInputs();

};

#endif // GLOBALOPTIONSDIALOG_H
