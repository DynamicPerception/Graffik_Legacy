#ifndef ADDNETDIALOG_H
#define ADDNETDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QColor>

#include "MoCoBus/omnetwork.h"

namespace Ui {
    class addNetDialog;
}

class addNetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addNetDialog(OMNetwork*, QWidget *parent = 0);
    ~addNetDialog();

    void updateSerialPorts();

private:
    Ui::addNetDialog *ui;
    QColor _thsColor;
    OMNetwork *_net;
    QWidget* _parent;

    void _setNetBackground(QColor);

public slots:
    void accept();


private slots:
    void on_colorSetButton_clicked();
    void on_rescanButton_clicked();
    void on_portCombo_currentIndexChanged(int);
    void _colorChange(const QColor & color);

};

#endif // ADDNETDIALOG_H
