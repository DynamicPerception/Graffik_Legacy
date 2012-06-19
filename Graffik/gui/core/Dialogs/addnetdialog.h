#ifndef ADDNETDIALOG_H
#define ADDNETDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QColor>

#include "MoCoBus/omnetwork.h"

namespace Ui {
    class AddNetDialog;
}

class AddNetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNetDialog(OMNetwork*, QWidget *parent = 0);
    ~AddNetDialog();

    void updateSerialPorts();

private:
    Ui::AddNetDialog *ui;
    QColor _thsColor;
    OMNetwork *_net;
    QWidget* _parent;
    QColorDialog* _colorDia;
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
