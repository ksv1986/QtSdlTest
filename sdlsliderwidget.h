#ifndef SDLSLIDERWIDGET_H
#define SDLSLIDERWIDGET_H

#include <QWidget>
#include <SDL_events.h>

class SdlSliderWidget : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;
    void reset();

public slots:
    void axisMoved(SDL_JoyAxisEvent event);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Sint16 value = 0;
};

#endif // SDLSLIDERWIDGET_H
