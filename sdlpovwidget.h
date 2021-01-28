#ifndef SDLPOVWIDGET_H
#define SDLPOVWIDGET_H

#include <QWidget>
#include <SDL_events.h>

class QPolygon;

class SdlPovWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SdlPovWidget(QWidget *parent = 0);
    void reset();

public slots:
    void povPressed(SDL_JoyHatEvent event);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawPovButton(QPainter &painter, int kx, int ky);
    void resizeCross();
    int X(int kx) const { return xoffset + kx*crossSize; }
    int Y(int ky) const { return yoffset + ky*crossSize; }

    Uint8 value = 0;
    int crossSize;
    int xoffset;
    int yoffset;
    QPolygon *povCross;
};

#endif // SDLPOVWIDGET_H
