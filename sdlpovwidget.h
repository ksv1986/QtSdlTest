#ifndef SDLPOVWIDGET_H
#define SDLPOVWIDGET_H

#include <QWidget>
#include <SDL_events.h>
#include <SDL_joystick.h>

class QPolygon;

class SdlPovWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SdlPovWidget(QWidget *parent = 0);
    void init(SDL_Joystick *joystick, int hat);
    void reset();

public slots:
    void povPressed(SDL_JoyHatEvent event);
    
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void drawPovButton(QPainter &painter, int kx, int ky);

private:
    void resizeCross();
    int X(int kx) { return xoffset + kx*crossSize; }

    int Y(int ky) { return yoffset + ky*crossSize; }

    SDL_JoystickID which;
    int hat;
    int value;
    int crossSize;
    int xoffset;
    int yoffset;
    QPolygon *povCross;
};

#endif // SDLPOVWIDGET_H
