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
    void axisMoved(const SDL_JoyAxisEvent &event);
    void joyDeviceAdded(const SDL_JoyDeviceEvent &event);
    void joyDeviceRemoved(const SDL_JoyDeviceEvent &event);
    void joystickButtonPressed(const SDL_JoyButtonEvent &event);
    void povPressed(const SDL_JoyHatEvent &event);
    void stopHaptic();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void closeHaptic();
    void closeJoystick();
    void setButtons(int num);
    void setHaptic(SDL_Haptic *haptic);
    void fillJoystickList();

    Ui::MainWindow *ui;
    SdlEventThread *eventThread;
    struct _SDL_Joystick *joystick;
    SDL_JoystickID which;
    SDL_Haptic *haptic;
    SDL_HapticEffect lr;
    int hat;
    int hapticId;
    int numAxis;
    int numButtons;
    int numHats;
};

#endif // MAINWINDOW_H
