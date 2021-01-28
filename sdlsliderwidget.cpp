#include "sdlsliderwidget.h"
#include <QPainter>
#include <QPen>

SdlSliderWidget::SdlSliderWidget(QWidget *parent)
    : QWidget{parent}
{
}

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
    int halfh = this->geometry().height()/2;
    int h = halfh * value/0x7FFF;
    if (h > 0) {
        painter.fillRect(0, halfh-h, geometry().width(), h, Qt::red);
    } else {
        painter.fillRect(0, halfh,   geometry().width(),-h, Qt::blue);
    }
}
