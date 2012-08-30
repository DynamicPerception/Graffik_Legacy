#ifndef OMAXISFILMOPTIONSDIALOG_H
#define OMAXISFILMOPTIONSDIALOG_H

#include <QDialog>

#include "axisoptions.h"


namespace Ui {
class AxisOptionsDialog;
}

class AxisOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    AxisOptionsDialog(AxisOptions* c_opts, unsigned short c_addr, QWidget *parent = 0);
    ~AxisOptionsDialog();
    
private slots:
    void _defaultComboChange(int p_idx);
    void _accept();

private:
    Ui::AxisOptionsDialog* ui;
    unsigned short m_addr;
    OMaxisOptions* m_opts;
    AxisOptions* m_optObj;

    void _initInputs();

};

#endif // OMAXISFILMOPTIONSDIALOG_H
