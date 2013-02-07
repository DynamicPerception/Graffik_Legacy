#ifndef MOTIONERRORDIALOG_H
#define MOTIONERRORDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QString>
#include <QList>

#include "motionerrorelement.h"
#include "core/Themer/singlethemer.h"

namespace Ui {
class MotionErrorDialog;
}

class MotionErrorDialog : public QDialog
{
    Q_OBJECT
    
public:
    MotionErrorDialog(QList<QString> c_errs, QWidget *parent = 0);
    ~MotionErrorDialog();
    
private:
    Ui::MotionErrorDialog *ui;
    QVBoxLayout* m_areaLayout;

};

#endif // MOTIONERRORDIALOG_H
