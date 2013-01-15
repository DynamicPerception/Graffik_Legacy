#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include "core/Themer/singlethemer.h"


namespace Ui {
class WelcomeDialog;
}

/** Welcome Dialog Class

  Presents a welcome dialog, primarily used for the first time a user
  runs the application, or the first run after clearing settings.

  @author
  C. A. Church
  */

class WelcomeDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit WelcomeDialog(QWidget *parent = 0);
    ~WelcomeDialog();
    
public slots:
    void on_cancelButton_clicked();
    void on_addButton_clicked();

signals:
    /** User Clicked Add Bus button */
    void addBus();

private:
    Ui::WelcomeDialog *ui;
};

#endif // WELCOMEDIALOG_H
