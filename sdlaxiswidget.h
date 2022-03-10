#ifndef QAXISWIDGET_H
#define QAXISWIDGET_H

#include <QWidget>
#include <SDL_events.h>

class SdlAxisWidget : public QWidget
{
    Q_OBJECT
public:
    static const int kCrossSize = 5;

    using QWidget::QWidget;
    void reset();

public slots:
    void xaxisMoved(const SDL_JoyAxisEvent &event);
    void yaxisMoved(const SDL_JoyAxisEvent &event);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawCross(QPainter &painter, int dx, int dy);

    Sint16 jx = 0;
    Sint16 jy = 0;
};

#endif // QAXISWIDGET_H
