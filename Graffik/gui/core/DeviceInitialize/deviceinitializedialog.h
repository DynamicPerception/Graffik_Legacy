#ifndef DEVICEINITIALIZEDIALOG_H
#define DEVICEINITIALIZEDIALOG_H

#include <QDialog>

namespace Ui {
class DeviceInitializeDialog;
}

class DeviceInitializeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DeviceInitializeDialog(QWidget *parent = 0);
    ~DeviceInitializeDialog();
    
private:
    Ui::DeviceInitializeDialog *ui;
};

#endif // DEVICEINITIALIZEDIALOG_H
