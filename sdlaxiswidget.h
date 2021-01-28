#ifndef QAXISWIDGET_H
#define QAXISWIDGET_H

#include <QWidget>
#include <SDL_events.h>

class SdlAxisWidget : public QWidget
{
    Q_OBJECT
public:
    static const int kCrossSize = 5;

    explicit SdlAxisWidget(QWidget *parent = 0);
    void reset();

public slots:
    void xaxisMoved(SDL_JoyAxisEvent event);
    void yaxisMoved(SDL_JoyAxisEvent event);

protected:
    void paintEvent(QPaintEvent *event);
    void drawCross(QPainter &painter, int dx, int dy);
private:
    Sint16 jx = 0;
    Sint16 jy = 0;
};

#endif // QAXISWIDGET_H
