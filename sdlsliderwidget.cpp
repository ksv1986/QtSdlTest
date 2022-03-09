#include "sdlsliderwidget.h"
#include <QPainter>
#include <QPen>

void SdlSliderWidget::reset()
{
    value = 0;
    update();
}

void SdlSliderWidget::axisMoved(SDL_JoyAxisEvent event)
{
    value = event.value;
    update();
}

void SdlSliderWidget::paintEvent(QPaintEvent *)
{
    if (!value)
        return;

    QPainter painter(this);
    int halfh = geometry().height() / 2;
    int h = halfh * value / SDL_MAX_SINT16;
    if (h > 0) {
        painter.fillRect(0, halfh-h, geometry().width(), h, Qt::red);
    } else {
        painter.fillRect(0, halfh,   geometry().width(),-h, Qt::blue);
    }
}
