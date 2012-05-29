#ifndef OMAXISFILMOPTIONSDIALOG_H
#define OMAXISFILMOPTIONSDIALOG_H

#include <QDialog>

#include "omaxisfilmoptions.h"


namespace Ui {
class OMAxisFilmOptionsDialog;
}

class OMAxisFilmOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    OMAxisFilmOptionsDialog(OMAxisFilmOptions* c_opts, unsigned short c_addr, QWidget *parent = 0);
    ~OMAxisFilmOptionsDialog();
    
private slots:
    void _defaultComboChange(int p_idx);
    void _accept();

private:
    Ui::OMAxisFilmOptionsDialog *ui;
    unsigned short m_addr;
    OMaxisOptions* m_opts;
    OMAxisFilmOptions* m_optObj;

    void _initInputs();

};

#endif // OMAXISFILMOPTIONSDIALOG_H