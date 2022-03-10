#ifndef SDLEVENTTHREAD_H
#define SDLEVENTTHREAD_H

#include <QThread>
#include <QMetaType>
#include <SDL_events.h>

Q_DECLARE_METATYPE(SDL_Event)
Q_DECLARE_METATYPE(SDL_JoyAxisEvent)
Q_DECLARE_METATYPE(SDL_JoyHatEvent)
Q_DECLARE_METATYPE(SDL_JoyButtonEvent)

class SdlEventThread : public QThread
{
    Q_OBJECT
public:
    using QThread::QThread;
    void stop();

signals:
    void axisEvent(const SDL_JoyAxisEvent &event);
    void hatEvent(const SDL_JoyHatEvent &event);
    void joyButtonEvent(const SDL_JoyButtonEvent &event);
    void joyDeviceAdded(const SDL_JoyDeviceEvent &event);
    void joyDeviceRemoved(const SDL_JoyDeviceEvent &event);
    void sdlEvent(const SDL_Event &event);

protected:
    void run() override;
};

#endif // SDLEVENTTHREAD_H
