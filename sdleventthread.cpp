#include "sdleventthread.h"

static const char __CLASS__[] = "EventThread";

void SdlEventThread::run()
{
    SDL_Event e;
    qDebug("%s: start event thread", __CLASS__);
    SDL_JoystickEventState(SDL_ENABLE);
    while (SDL_WaitEvent(&e)) {
        //qDebug("%s: event type %x", __CLASS__, e.type);

        switch (e.type) {
        case SDL_QUIT:
            qDebug("%s: end event thread", __CLASS__);
            return;
        case SDL_JOYAXISMOTION:
            emit axisEvent(e.jaxis);
            break;
        case SDL_JOYHATMOTION:
            emit hatEvent(e.jhat);
            break;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            emit joyButtonEvent(e.jbutton);
            break;
        case SDL_JOYDEVICEADDED:
            emit joyDeviceAdded(e.jdevice);
            break;
        case SDL_JOYDEVICEREMOVED:
            emit joyDeviceRemoved(e.jdevice);
            break;
        default:
            emit sdlEvent(e);
        }
    }
}

void SdlEventThread::stop()
{
    SDL_Event e;
    e.type = SDL_QUIT;
    SDL_PushEvent(&e);
    wait();
}
