#ifndef SDLSLIDERWIDGET_H
#define SDLSLIDERWIDGET_H

#include <QWidget>
#include <SDL_events.h>
#include <SDL_joystick.h>

class SdlSliderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SdlSliderWidget(QWidget *parent = 0);
    void init(SDL_Joystick *joystick, int axis);
    void reset();

public slots:
    void axisMoved(SDL_JoyAxisEvent event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    SDL_JoystickID which;
    int axis;
    int value;
};

#endif // SDLSLIDERWIDGET_H
