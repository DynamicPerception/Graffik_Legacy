#ifndef SKINNEDDIAL_H
#define SKINNEDDIAL_H

#include <QDial>
#include <QPixmap>
#include <QPainter>

class SkinnedDial : public QDial
{
    Q_OBJECT
    Q_PROPERTY(QPixmap backgroundImage READ backgroundImage WRITE setBackgroundImage DESIGNABLE true)
    Q_PROPERTY(QPixmap needleImage READ needleImage WRITE setNeedleImage DESIGNABLE true)
    Q_PROPERTY(float maxAngle READ maxAngle WRITE setMaxAngle DESIGNABLE true)

public:
    explicit SkinnedDial(QWidget *parent = 0);
    SkinnedDial(QPixmap* c_back, QPixmap* c_needle, float c_angle, QWidget *parent = 0);

    ~SkinnedDial();

    QPixmap backgroundImage();
    void setBackgroundImage(QPixmap p_img);

    QPixmap needleImage();
    void setNeedleImage(QPixmap p_img);

    float maxAngle();
    void setMaxAngle(float p_angle);

protected:

    void paintEvent(QPaintEvent *pe);

signals:
    
public slots:
    
private:

    QPixmap* m_background;
    QPixmap* m_needle;
    QPainter::RenderHint m_paintFlags;

    float m_maxDeg;

    QPixmap _rotatePix(QPixmap* p_pix, float p_deg, bool p_dir);

};

#endif // SKINNEDDIAL_H
