#ifndef CAMERACONTROLDIALOG_H
#define CAMERACONTROLDIALOG_H

#include <QDialog>

namespace Ui {
class CameraControlDialog;
}

class CameraControlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CameraControlDialog(QWidget *parent = 0);
    ~CameraControlDialog();
    
private:
    Ui::CameraControlDialog *ui;
};

#endif // CAMERACONTROLDIALOG_H
