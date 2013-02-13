#include "trackinfodialog.h"
#include "ui_trackinfodialog.h"

TrackInfoDialog::TrackInfoDialog(OMfilmAxisParams *c_parms, QWidget *parent) : QDialog(parent), ui(new Ui::TrackInfoDialog) {
    ui->setupUi(this);

    QString start;
    QString end;
    QString ease;

    if( c_parms->startTm == 0 )
        start = TID_STR_FSTR;
    else
        start = TimeConverter::stringify(c_parms->startTm);

    if( c_parms->endTm == 0 )
        end = TID_STR_FEND;
    else
        end = TimeConverter::stringify(c_parms->endTm);

    if( c_parms->easing == OM_MOT_QUAD )
        ease = TID_STR_EASQ;
    else if( c_parms->easing == OM_MOT_QUADINV )
        ease = TID_STR_EASI;
    else
        ease = TID_STR_EASL;

    QString contents =
            TID_STR_DIST   + QString::number(c_parms->endDist) + "\n" +
            TID_STR_STARTM + start + " (" + QString::number(c_parms->startTm) + ")\n" +
            TID_STR_ENDTM  + end + " (" + QString::number(c_parms->endTm) + ")\n" +
            TID_STR_EASE   + ease + "\n" +
            TID_STR_ACTM   + TimeConverter::stringify(c_parms->accelTm) + " (" + QString::number(c_parms->accelTm) + ")\n" +
            TID_STR_DCTM   + TimeConverter::stringify(c_parms->decelTm) + " (" + QString::number(c_parms->decelTm) + ")\n";


    ui->textBrowser->setText(contents);

    setStyleSheet(SingleThemer::getStyleSheet("trackinfo"));
}

TrackInfoDialog::~TrackInfoDialog() {
    delete ui;
}
