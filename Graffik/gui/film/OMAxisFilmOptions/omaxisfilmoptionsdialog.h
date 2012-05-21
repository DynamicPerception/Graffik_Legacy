#ifndef OMAXISFILMOPTIONSDIALOG_H
#define OMAXISFILMOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OMAxisFilmOptionsDialog;
}

class OMAxisFilmOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OMAxisFilmOptionsDialog(QWidget *parent = 0);
    ~OMAxisFilmOptionsDialog();
    
private:
    Ui::OMAxisFilmOptionsDialog *ui;
};

#endif // OMAXISFILMOPTIONSDIALOG_H
