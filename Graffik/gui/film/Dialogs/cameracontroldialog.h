#ifndef CAMERACONTROLDIALOG_H
#define CAMERACONTROLDIALOG_H

#include <QDialog>

#include "film/FilmParameters/filmparameters.h"

namespace Ui {
class CameraControlDialog;
}

class CameraControlDialog : public QDialog
{
    Q_OBJECT
    
public:
    CameraControlDialog(FilmParameters* c_params, QWidget *parent = 0);
    ~CameraControlDialog();

signals:
    void intervalControlChanged(bool p_state);
    void fpsChanged(unsigned short p_fps);

public slots:
    void on_bulbCheck_stateChanged(int p_state);
    void on_focusCheck_stateChanged(int p_state);
    void on_intervalCheck_stateChanged(int p_state);
    void on_autoCheck_stateChanged(int p_state);

    void accept();

private:
    Ui::CameraControlDialog *ui;

    FilmParameters* m_params;

    void _setupInputs();
};

#endif // CAMERACONTROLDIALOG_H
