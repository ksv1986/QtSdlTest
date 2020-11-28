#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <stdio.h>

#include <SDL.h>
#include <SDL_haptic.h>

#define APPNAME "QtSdlTest"

#define E(x) {#x, x}
struct haptic_flags_t {
    const char *name;
    unsigned int flag;
} haptic_flags[] = {
    E(SDL_HAPTIC_CONSTANT),
    E(SDL_HAPTIC_SINE),
    E(SDL_HAPTIC_LEFTRIGHT),
    E(SDL_HAPTIC_TRIANGLE),
    E(SDL_HAPTIC_SAWTOOTHUP),
    E(SDL_HAPTIC_SAWTOOTHDOWN),
    E(SDL_HAPTIC_RAMP),
    E(SDL_HAPTIC_SPRING),
    E(SDL_HAPTIC_DAMPER),
    E(SDL_HAPTIC_INERTIA),
    E(SDL_HAPTIC_FRICTION),
    E(SDL_HAPTIC_CUSTOM),
    E(SDL_HAPTIC_GAIN),
    E(SDL_HAPTIC_AUTOCENTER),
    E(SDL_HAPTIC_STATUS),
    E(SDL_HAPTIC_PAUSE),
    {NULL, 0}
};

static void joystickDescribe(int index, SDL_Joystick *joystick)
{
    const char *name = SDL_JoystickName(joystick);
    qDebug("Joystick[%d]: %s", index, name ? name : "Unknown");

    char guid[64];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick),
                              guid, sizeof (guid));
    qDebug(" axes: %d", SDL_JoystickNumAxes(joystick));
    qDebug(" balls: %d", SDL_JoystickNumBalls(joystick));
    qDebug(" hats: %d", SDL_JoystickNumHats(joystick));
    qDebug(" buttons: %d", SDL_JoystickNumButtons(joystick));
    qDebug(" instance id: %d", SDL_JoystickInstanceID(joystick));
    qDebug(" guid: %s", guid);
    qDebug(" game controller: %d", SDL_IsGameController(index));

    SDL_Haptic * haptic = SDL_HapticOpenFromJoystick(joystick);
    if (!haptic) {
        qDebug(" haptic: no");
        qDebug("%s", "");
        return;
    }
    unsigned int flags = SDL_HapticQuery(haptic);
    qDebug(" haptic: %d/%d effects %d axes (%08x)",
           SDL_HapticNumEffectsPlaying(haptic),
           SDL_HapticNumEffects(haptic),
           SDL_HapticNumAxes(haptic),
           flags);
    for (int i = 0; haptic_flags[i].flag; ++i) {
        if (flags & haptic_flags[i].flag)
            qDebug("  %s", haptic_flags[i].name);
    }
    qDebug(" rumble: %d", SDL_HapticRumbleSupported(haptic));
    qDebug("%s", "");
    SDL_HapticStopAll(haptic);
    SDL_HapticClose(haptic);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<SDL_Event>("SDL_Event");
    qRegisterMetaType<SDL_JoyAxisEvent>("SDL_JoyAxisEvent");
    qRegisterMetaType<SDL_JoyHatEvent>("SDL_JoyHatEvent");
    qRegisterMetaType<SDL_JoyButtonEvent>("SDL_JoyButtonEvent");

    int rv = SDL_Init(SDL_INIT_JOYSTICK|SDL_INIT_HAPTIC);
    if (rv) {
        const char *err = SDL_GetError();
        qDebug("%s: failed to init SDL: %s", APPNAME, err);
        QMessageBox::information(0, "QtSdlTest", QString("Failed to init SDL: ") + err);
        return rv;
    }

    qDebug("Number of attached joysticks: %d", SDL_NumJoysticks());
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        if (!joystick)
            continue;

        joystickDescribe(i, joystick);
        SDL_JoystickClose(joystick);
    }

    int index = 0;
    if (argc > 1)
        index = atoi(argv[1]);
    if (index >= SDL_NumJoysticks()) {
        qDebug("joystick %d is not found, using 0 instead", index);
        index = 0;
    }

    MainWindow w;
    SDL_Joystick *joystick = SDL_JoystickOpen(index);
    if (joystick) {
        w.setJoystick(index);
        SDL_JoystickClose(joystick);
    }

    w.show();
    return a.exec();
}
