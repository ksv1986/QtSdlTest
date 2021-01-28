#include "sdlaxiswidget.h"
#include <QPainter>

SdlAxisWidget::SdlAxisWidget(QWidget *parent)
    : QWidget{parent}
    , which{-1}
{
}

void SdlAxisWidget::init(SDL_Joystick *joystick, int xaxis, int yaxis)
{
    which = SDL_JoystickInstanceID(joystick);
    this->xaxis = xaxis;
    this->yaxis = yaxis;
    jx = SDL_JoystickGetAxis(joystick, xaxis);
    jy = SDL_JoystickGetAxis(joystick, yaxis);
    update();
}

void SdlAxisWidget::reset()
{
    which = -1;
    update();
}

void SdlAxisWidget::axisMoved(SDL_JoyAxisEvent event)
{
    if (event.which != which)
        return;
    if (event.axis == xaxis)
        jx = event.value;
    else if (event.axis == yaxis)
        jy = event.value;
    else
        return;
    update();
}

void SdlAxisWidget::paintEvent(QPaintEvent *)
{
    // jx and jy are from int16_t diapasone. Map them to [0-size]:
    int dx = width()  / 2;
    int dy = height() / 2;
    dx += dx*jx / 0x8000;
    dy += dy*jy / 0x8000;
    QPainter painter(this);
    painter.setPen(Qt::black);
    drawCross(painter, dx, dy);
}

void SdlAxisWidget::drawCross(QPainter &painter, int dx, int dy) {
    painter.drawLine(dx-kCrossSize,dy,dx+kCrossSize,dy);
    painter.drawLine(dx,dy-kCrossSize,dx,dy+kCrossSize);
}
