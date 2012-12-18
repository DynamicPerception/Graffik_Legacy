#ifndef FLICKNUMBERITEM_H
#define FLICKNUMBERITEM_H

#include <QWidget>
#include <QListWidgetItem>
#include <QString>
#include <QLabel>
#include <QHBoxLayout>

class FlickNumberItem : public QWidget
{

    Q_OBJECT

public:
    explicit FlickNumberItem(QString c_first, QString c_second, QWidget *parent = 0);
    ~FlickNumberItem();
    
    //QWidget* widget();

    void setFirst(QString p_str);
    void setSecond(QString p_str);

private:
   // QWidget*     m_widget;
    QLabel*      m_first;
    QLabel*      m_second;
    QHBoxLayout* m_layout;
};

#endif // FLICKNUMBERITEM_H
