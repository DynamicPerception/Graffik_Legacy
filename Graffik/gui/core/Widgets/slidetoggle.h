#ifndef SLIDETOGGLE_H
#define SLIDETOGGLE_H

#include <QSlider>

class SlideToggle : public QSlider
{
    Q_OBJECT

public:
    explicit SlideToggle(QWidget *parent = 0);
    SlideToggle(Qt::Orientation orientation, QWidget *parent = 0);

signals:
    void selected(int p_val);

public slots:

private slots:
    void _getValue(int p_val);

private:
    void _setup();

};

#endif // SLIDETOGGLE_H
