#include "sdlpovwidget.h"
#include <QPainter>
#include <QPolygon>

void SdlPovWidget::reset()
{
    update();
}

void SdlPovWidget::povPressed(const SDL_JoyHatEvent &event)
{
    value = event.value;
    update();
}

/*     2 - 3
 *     |   |
 * 0 - 1   4 - 5
 * |           |
 * 11- 10  7 - 6
 *     |   |
 *     9 - 8
 *     <---> = crossSize
 */
void SdlPovWidget::resizeCross()
{
    crossSize = std::min(width(), height()) / 3;
    xoffset   = ( width() - 3*crossSize) / 2;
    yoffset   = (height() - 3*crossSize) / 2;

    povCross.setPoint( 0, X(0), Y(1));
    povCross.setPoint( 1, X(1), Y(1));
    povCross.setPoint( 2, X(1), Y(0));
    povCross.setPoint( 3, X(2), Y(0));
    povCross.setPoint( 4, X(2), Y(1));
    povCross.setPoint( 5, X(3), Y(1));
    povCross.setPoint( 6, X(3), Y(2));
    povCross.setPoint( 7, X(2), Y(2));
    povCross.setPoint( 8, X(2), Y(3));
    povCross.setPoint( 9, X(1), Y(3));
    povCross.setPoint(10, X(1), Y(2));
    povCross.setPoint(11, X(0), Y(2));
    povCross.setPoint(12, X(0), Y(1));
}

void SdlPovWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resizeCross();
}

void SdlPovWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(isEnabled()
        ? Qt::gray
        : Qt::lightGray);
    p.drawPolyline(povCross);
    if (!isEnabled())
        return;
    if (value & SDL_HAT_UP) {
        drawPovButton(p, 1, 0);
        p.drawLine(X(1)+1, Y(1),   X(2)-1, Y(1));
    }
    if (value & SDL_HAT_RIGHT) {
        drawPovButton(p, 2, 1);
        p.drawLine(X(2),   Y(1)+1, X(2),   Y(2)-1);
    }
    if (value & SDL_HAT_DOWN) {
        drawPovButton(p, 1, 2);
        p.drawLine(X(1)+1, Y(2),   X(2)-1, Y(2));
    }
    if (value & SDL_HAT_LEFT) {
        drawPovButton(p, 0, 1);
        p.drawLine(X(1),   Y(1)+1, X(1),   Y(2)-1);
    }
}

void SdlPovWidget::drawPovButton(QPainter &painter, int kx, int ky)
{
    painter.fillRect(X(kx)+1, Y(ky)+1, crossSize-1, crossSize-1, Qt::gray);
}
