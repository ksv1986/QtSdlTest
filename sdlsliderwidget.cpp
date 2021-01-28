#include "sdlsliderwidget.h"
#include <QPainter>
#include <QPen>

SdlSliderWidget::SdlSliderWidget(QWidget *parent)
    : QWidget{parent}
    , which{-1}
    , axis{-1}
    , value{0}
{
}

void SdlSliderWidget::init(SDL_Joystick *joystick, int axis)
{
    this->axis = axis;
    which = SDL_JoystickInstanceID(joystick);
    value = SDL_JoystickGetAxis(joystick, axis);
    update();
}

void SdlSliderWidget::reset()
{
    which = -1;
    update();
}

void SdlSliderWidget::axisMoved(SDL_JoyAxisEvent event)
{
    if (event.which != which)
        return;
    if (event.axis != axis)
        return;
    value = event.value;
    update();
}

void SdlSliderWidget::paintEvent(QPaintEvent *)
{
    if (!value)
        return;

    QPainter painter(this);
    int halfh = this->geometry().height()/2;
    int h = halfh * value/0x7FFF;
    if (h > 0) {
        painter.fillRect(0, halfh-h, geometry().width(), h, Qt::red);
    } else {
        painter.fillRect(0, halfh,   geometry().width(),-h, Qt::blue);
    }
}
