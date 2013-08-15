#ifndef QAXISWIDGET_H
#define QAXISWIDGET_H

#include <QWidget>
#include <SDL_events.h>
#include <SDL_joystick.h>

class SdlAxisWidget : public QWidget
{
    Q_OBJECT
public:
    static const int kCrossSize = 5;

    explicit SdlAxisWidget(QWidget *parent = 0);
    void init(SDL_Joystick *joystick, int xaxis, int yaxis);
    void reset();

public slots:
    void axisMoved(SDL_JoyAxisEvent event);

protected:
    void paintEvent(QPaintEvent *event);
    void drawCross(QPainter &painter, int dx, int dy);
private:
    SDL_JoystickID which;
    int xaxis;
    int yaxis;
    int jx;
    int jy;
};

#endif // QAXISWIDGET_H
