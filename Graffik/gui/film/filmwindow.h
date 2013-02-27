/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>
#include <QHash>
#include <QVBoxLayout>
#include <QPainterPath>
#include <QPaintEvent>
#include <QProgressDialog>
#include <QFileDialog>


#include "MoCoBus/omnetwork.h"

#include "core/Themer/singlethemer.h"

#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Dialogs/errordialog.h"

#include "core/Utilities/timeconverter.h"
#include "core/Utilities/flicknumber.h"
#include "core/Widgets/slidetoggle.h"

#include "film/JogControlManager/jogcontrolpanel.h"
#include "film/MotionArea/motionbase.h"
#include "film/MotionArea/motiontape.h"
#include "film/MotionArea/motionsection.h"
#include "film/MotionArea/sectionresizefilter.h"
#include "film/MotionArea/notrackswidget.h"
#include "film/FilmParameters/filmparameters.h"
#include "film/Dialogs/cameracontroldialog.h"
#include "film/FilmExec/filmexec.h"
#include "film/FilmExec/filmtimemanager.h"
#include "film/FilmFileHandler/filmfilehandler.h"




namespace Ui {
    class FilmWindow;
}

/** Film Window Widget

  The Film Window Widget is the base screen for the Film Workflow

  @author
  C. A. Church
  */

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
    void on_rewindButton_clicked();
    void on_forwardButton_clicked();
    void on_frameRwdButton_clicked();
    void on_frameFwdButton_clicked();

    // cam control

    void on_camControlSlider_selected(int p_val);
    void on_camSetBut_clicked();

    void on_plugJogButton_clicked();


    // spinbox handlers

    void on_filmHHSpin_valueChanged(unsigned int p_val);
    void on_filmMMSpin_valueChanged(unsigned int p_val);
    void on_filmSSSpin_valueChanged(unsigned int p_val);
    void on_realHHSpin_valueChanged(unsigned int p_val);
    void on_realMMSpin_valueChanged(unsigned int p_val);
    void on_realSSSpin_valueChanged(unsigned int p_val);

    void error(QString p_err);

    void filmParamsChanged();

    // handling global save/load buttons
    void load();
    void save();

    // handling timeline clicks
    void timelineClicked(unsigned long p_time);

private slots:
    void _drawNewAxis(OMdeviceInfo* p_dev);
    void _eraseAxis(QString p_bus, unsigned short p_addr);
    void _playStatus(bool p_stat, unsigned long p_time);
    void _filmStarted();
    void _busyCanceled();
    void _shuttleComplete();
    void _themeChanged();
    void _emergencyStop();

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
    FilmTimeManager* m_time;
    MotionTape* m_tape;
    MotionSection* m_motion;
    SectionResizeFilter* m_filter;
    QProgressDialog* m_busy;
    JogControlPanel* m_jcp;
    NoTracksWidget* m_notw;

    QWidget* m_areaViewPort;
    QVBoxLayout* m_areaLayout;
    QHash<unsigned short, MotionBase*> m_areaBlocks;

        // internal status registers
    static const int s_Enable   = 1;
    static const int s_Disable  = 2;
    static const int s_Pause    = 3;
    static const int s_Play     = 4;
    static const int s_DisPres  = 5;

    bool m_error;
    bool m_spinsPrepped;
    bool m_ignoreUpdate;
    bool m_notwShown;

    unsigned long m_curFrameShot;

    void _displayCamControl();
    void _enableCamControl(bool p_en = true);
    void _showFilmTime();
    void _prepInputs();
    void _showTotalFrames(unsigned long p_len, unsigned long p_val, bool p_type = false);

    void _changeTime(int p_which, int p_pos, int p_val);
    void _calcAutoFilmTime();
    void _checkFilmTimeConstraint();
    QList<unsigned short> _checkSMSMovements(OMfilmParams* p_params);

    void _setPlayButtonStatus(int p_stat);
    void _setStopButtonStatus(int p_stat);

    void _filmTimeDisplay(unsigned long p_ms);

    void _redrawMotionOverlay();
    void _inputEnable(bool p_stat);

    void _popTimeDisplay(QLabel* p_label, int p_time);
};

#endif // FILMWINDOW_H
