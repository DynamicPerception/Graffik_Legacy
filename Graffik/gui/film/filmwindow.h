#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>
#include <QHash>
#include <QVBoxLayout>
#include <QPainterPath>
#include <QPaintEvent>
#include <QProgressDialog>

#include "MoCoBus/omnetwork.h"

#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Dialogs/errordialog.h"

#include "core/Utilities/timeconverter.h"

#include "film/JogControlManager/jogcontrolpanel.h"
#include "film/MotionArea/motionbase.h"
#include "film/MotionArea/motiontape.h"
#include "film/MotionArea/motionsection.h"
#include "film/MotionArea/sectionresizefilter.h"
#include "film/FilmParameters/filmparameters.h"
#include "film/Dialogs/cameracontroldialog.h"
#include "film/FilmExec/filmexec.h"

namespace Ui {
    class FilmWindow;
}

class FilmWindow : public QWidget
{
    Q_OBJECT
    
public:
    FilmWindow(OMNetwork* c_net, AxisOptions* c_opts, GlobalOptions* c_gopts, QWidget *parent = 0);
    ~FilmWindow();

public slots:

  // set automatic connections

    // shuttle controls

    void on_playButton_clicked();
    void on_stopButton_clicked();

    // cam control

    void on_camControlCheckBox_stateChanged(int p_state);
    void on_camSetBut_clicked();

    void on_plugJogButton_clicked();

    // spinbox handlers

    void on_filmHHSpin_valueChanged(int p_val);
    void on_filmMMSpin_valueChanged(int p_val);
    void on_filmSSSpin_valueChanged(int p_val);
    void on_realHHSpin_valueChanged(int p_val);
    void on_realMMSpin_valueChanged(int p_val);
    void on_realSSSpin_valueChanged(int p_val);

    void error(QString p_err);

private slots:
    void _drawNewAxis(OMdeviceInfo* p_dev);
    void _eraseAxis(QString p_bus, unsigned short p_addr);
    void _playStatus(bool p_stat, unsigned long p_time);
    void _filmStarted();
    void _busyCanceled();

signals:
        /** This signal is reflected from the drawn motion areas to indicate
            the location of their borders */
    void motionAreaBorders(int p_leftX, int p_rightX);

        /** This signal is sent when the film playing status is changed, indicating what
          the current status is */
    void playStatusChange(bool p_stat);

private:
    Ui::FilmWindow *ui;
    OMNetwork* m_net;
    AxisOptions* m_opts;
    GlobalOptions* m_gopts;
    FilmParameters* m_params;
    FilmExec* m_exec;
    MotionTape* m_tape;
    MotionSection* m_motion;
    SectionResizeFilter* m_filter;
    QProgressDialog* m_busy;
    JogControlPanel* m_jcp;

    QVBoxLayout* m_areaLayout;
    QHash<unsigned short, MotionBase*> m_areaBlocks;

        // internal status registers
    static const int s_Enable   = 1;
    static const int s_Disable  = 2;
    static const int s_Pause    = 3;
    static const int s_Play     = 4;

    bool m_error;

    void _enableCamControl(bool p_en = true);
    void _showFilmTime();
    void _prepInputs();

    void _changeTime(int p_which, int p_pos, int p_val);
    void _calcAutoFilmTime();
    void _checkFilmTimeConstraint();

    void _setPlayButtonStatus(int p_stat);
    void _setStopButtonStatus(int p_stat);

    void _filmTimeDisplay(unsigned long p_ms);

    void _redrawMotionOverlay();
    void _inputEnable(bool p_stat);
};

#endif // FILMWINDOW_H
