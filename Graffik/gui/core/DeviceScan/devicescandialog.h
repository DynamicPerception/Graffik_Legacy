#ifndef DEVICESCANDIALOG_H
#define DEVICESCANDIALOG_H

#include <QDialog>
#include <QString>


namespace Ui {
class DeviceScanDialog;
}

class DeviceScanDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DeviceScanDialog(QWidget *parent = 0);
    ~DeviceScanDialog();

    void enableConfirm(bool p_enable);
    void addNote(QString p_note);
    void totalNodes(int p_nodes);
    void scannedNodes(int p_nodes);
    void nodesFound(bool p_found);

public slots:
    void on_doneButton_clicked();
    void on_cancelButton_clicked();
        
private:
    Ui::DeviceScanDialog *ui;
};

#endif // DEVICESCANDIALOG_H
