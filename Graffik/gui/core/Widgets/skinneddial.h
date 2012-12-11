#ifndef SKINNEDDIAL_H
#define SKINNEDDIAL_H

#include <QDial>
#include <QPixmap>

class SkinnedDial : public QDial
{
    Q_OBJECT
    Q_PROPERTY(QPixmap backgroundImage READ backgroundImage WRITE setBackgroundImage DESIGNABLE true)
    Q_PROPERTY(QPixmap needleImage READ needleImage WRITE setNeedleImage DESIGNABLE true)

public:
    explicit SkinnedDial(QWidget *parent = 0);
    ~SkinnedDial();

    QPixmap backgroundImage();
    void setBackgroundImage(QPixmap p_img);

    QPixmap needleImage();
    void setNeedleImage(QPixmap p_img);

protected:

    void paintEvent(QPaintEvent *pe);

signals:
    
public slots:
    
private:

    QPixmap* m_background;
    QPixmap* m_needle;
};

#endif // SKINNEDDIAL_H
