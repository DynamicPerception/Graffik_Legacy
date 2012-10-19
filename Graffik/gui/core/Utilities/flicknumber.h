#ifndef FLICKNUMBER_H
#define FLICKNUMBER_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPainter>
#include <QIcon>


#include "flickcharm.h"

namespace Ui {
class FlickNumber;
}

class FlickNumber : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlickNumber(QWidget *parent = 0);
    FlickNumber(unsigned int c_count, QWidget *parent = 0);
    ~FlickNumber();

    void setCount(unsigned int p_count);
    void setValue(unsigned int p_val);
    unsigned int value();
    
public slots:
    void flickSettled();


signals:
    void valueChanged(unsigned int p_val);

private:
    Ui::FlickNumber *ui;
    FlickCharm* m_fcp;

    unsigned int m_cnt;

    void _setFlick();
    QImage _genIconImage(unsigned int p_val);
};

#endif // FLICKNUMBER_H
