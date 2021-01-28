#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SDL_haptic.h>
#include "sdleventthread.h"

namespace Ui {
class MainWindow;
}

struct _SDL_Joystick;
class QLabel;
class SdlAxisWidget;
class SdlPovWidget;
class SdlSliderWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setJoystick(int index);

private slots:
    void axisMoved(SDL_JoyAxisEvent event);
    void joyDeviceAdded(SDL_JoyDeviceEvent event);
    void joyDeviceRemoved(SDL_JoyDeviceEvent event);
    void joystickButtonPressed(SDL_JoyButtonEvent event);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void closeHaptic();
    void closeJoystick();
    void setButtons(int num);
    void setHaptic(SDL_Haptic *haptic);
    void setPov(SdlPovWidget *widget, int hat);
    void setAxis(SdlAxisWidget *widget, int xaxis, int yaxis);
    void setSlider(SdlSliderWidget *widget, int axis);
    void fillJoystickList();

    Ui::MainWindow *ui;
    SdlEventThread *eventThread;
    struct _SDL_Joystick *joystick;
    SDL_Haptic *haptic;
    SDL_HapticEffect lr;
    int hapticId;
    int numAxis;
    int numButtons;
    int numHats;

private slots:
    void stopHaptic();
};

#endif // MAINWINDOW_H
