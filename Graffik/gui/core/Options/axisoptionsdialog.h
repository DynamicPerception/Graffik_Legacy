#ifndef OMAXISFILMOPTIONSDIALOG_H
#define OMAXISFILMOPTIONSDIALOG_H

#include <QDialog>

#include "axisoptions.h"

#define AOD_STR_MAST    "By setting this device as the timing master, you will automatically unset any existing master device. Do you want to do this?"
#define AOD_STR_UNMAST  "If you unset this device as the timing master, you must manually select and edit another device and configure it as timing master for synchronization to work. Do you want to do this?"

#define AOD_STR_PAN     "VX1 Pan"
#define AOD_STR_TILT    "VX1 Tilt"
#define AOD_STR_SLIDE   "VX1 Slider"
#define AOD_STR_CUST    "Custom"

#define AOD_STR_ROT     "Rotary"
#define AOD_STR_LIN     "Linear"

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
