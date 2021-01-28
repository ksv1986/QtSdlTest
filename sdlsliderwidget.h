#ifndef SDLSLIDERWIDGET_H
#define SDLSLIDERWIDGET_H

#include <QWidget>
#include <SDL_events.h>

class SdlSliderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SdlSliderWidget(QWidget *parent = 0);
    void reset();

public slots:
    void axisMoved(SDL_JoyAxisEvent event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Sint16 value = 0;
};

#endif // SDLSLIDERWIDGET_H
