#ifndef ADDRESSNOTFOUNDDIALOG_H
#define ADDRESSNOTFOUNDDIALOG_H

#include <QDialog>
#include <QHash>


namespace Ui {
class AddressNotFoundDialog;
}

class AddressNotFoundDialog : public QDialog
{
    Q_OBJECT
    
public:
    AddressNotFoundDialog(unsigned short c_addr, QHash<unsigned short, QString> c_list, QWidget *parent = 0);
    ~AddressNotFoundDialog();

    void accept();
    
private:
    Ui::AddressNotFoundDialog *ui;

    QHash<unsigned short, QString> m_replace;
};

#endif // ADDRESSNOTFOUNDDIALOG_H
