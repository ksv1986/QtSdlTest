#include "sdlaxiswidget.h"
#include <QPainter>

void SdlAxisWidget::reset()
{
    jx = jy = 0;
    update();
}

void SdlAxisWidget::xaxisMoved(const SDL_JoyAxisEvent &event)
{
    jx = event.value;
    update();
}

void SdlAxisWidget::yaxisMoved(const SDL_JoyAxisEvent &event)
{
    jy = event.value;
    update();
}

void SdlAxisWidget::paintEvent(QPaintEvent *)
{
    // jx and jy are from int16_t diapasone. Map them to [0-size]:
    int dx = width()  / 2;
    int dy = height() / 2;
    dx += dx*jx / SDL_MAX_SINT16;
    dy += dy*jy / SDL_MAX_SINT16;
    QPainter painter(this);
    painter.setPen(Qt::black);
    drawCross(painter, dx, dy);
}

void SdlAxisWidget::drawCross(QPainter &painter, int dx, int dy) {
    painter.drawLine(dx-kCrossSize,dy,dx+kCrossSize,dy);
    painter.drawLine(dx,dy-kCrossSize,dx,dy+kCrossSize);
}
