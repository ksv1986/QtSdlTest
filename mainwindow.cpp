#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCloseEvent>
#include <QTimer>
#include <QSignalMapper>
#include <QGridLayout>

#include <SDL_haptic.h>

const Uint16 kHapticMagnitude = SDL_MAX_SINT16;
const int kHapticLength = 500;
const int kSafeTimeout  = 50;
const int kItemsInRow = 4;

static const char __CLASS__[] = "MainWindow";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui{new Ui::MainWindow}
    , eventThread{new SdlEventThread{this}}
    , joystick{0}
    , haptic{0}
    , hapticId{-1}
    , numAxis{0}
    , numButtons{0}
    , numHats{0}
{
    ui->setupUi(this);

    fillJoystickList();

    connect(eventThread, &SdlEventThread::joyDeviceAdded,
            this, &MainWindow::joyDeviceAdded);
    connect(eventThread, &SdlEventThread::joyDeviceRemoved,
            this, &MainWindow::joyDeviceRemoved);
    connect(ui->choosePadBox, QOverload<int>::of(&QComboBox::activated), this, [this](int index){
        setJoystick(index - 1);
    });

    eventThread->start();
}

MainWindow::~MainWindow()
{
    closeJoystick();
    eventThread->stop();
    delete ui;
}

void MainWindow::closeHaptic()
{
    stopHaptic();
    if (!haptic)
        return;
    SDL_HapticClose(haptic);
    haptic = 0;
}

void MainWindow::closeJoystick()
{
    closeHaptic();

    if (!joystick)
        return;

    qDebug("%s: close joystick %d %s", __CLASS__,
           SDL_JoystickInstanceID(joystick), SDL_JoystickName(joystick));

    disconnect(eventThread, &SdlEventThread::joyButtonEvent,
        this, &MainWindow::joystickButtonPressed);
    disconnect(eventThread, &SdlEventThread::hatEvent,
        this, &MainWindow::povPressed);
    disconnect(eventThread, &SdlEventThread::axisEvent,
        this, &MainWindow::axisMoved);

    while (auto child = ui->buttonsLayout->takeAt(0)) {
        delete child;
    }

    joystick = 0;
    numAxis    = 0;
    numButtons = 0;
    numHats    = 0;
    which = -1;

    ui->xyaxis->reset();
    ui->zaxis->reset();
    ui->axis4->reset();
    ui->axis5->reset();
    ui->pov->reset();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    eventThread->stop();
    event->accept();
}

void MainWindow::setButtons(int num)
{
    for (int i = 0; i<num; i++) {
        QPushButton *b = new QPushButton(this);
        b->setText(QString::number(i+1));
        ui->buttonsLayout->addWidget(b, i / kItemsInRow, i % kItemsInRow);
    }
}

void MainWindow::fillJoystickList()
{
    while (ui->choosePadBox->count() > 1)
        ui->choosePadBox->removeItem(1);

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        ui->choosePadBox->addItem(SDL_JoystickNameForIndex(i));
}

void MainWindow::setJoystick(int index)
{
    closeJoystick();
    joystick = SDL_JoystickOpen(index);
    if (!joystick)
        return;

    which = SDL_JoystickInstanceID(joystick);
    ui->choosePadBox->setCurrentIndex(index + 1);

    qDebug("%s: setup joystick %d %s", __CLASS__,
           SDL_JoystickInstanceID(joystick), SDL_JoystickName(joystick));

    numAxis    = SDL_JoystickNumAxes(joystick);
    numButtons = SDL_JoystickNumButtons(joystick);
    numHats    = SDL_JoystickNumHats(joystick);

    hat = 0;
    ui->pov->setEnabled(numHats > 0);
    setButtons(numButtons);

    int axisOffset = numButtons;
    while (axisOffset % kItemsInRow) ++axisOffset;
    for (int i = 0; i<numAxis; i++) {
        QLabel *l = new QLabel(this);
        l->setText(QString("[%1]").arg(i));
        ui->buttonsLayout->addWidget(l, (axisOffset + i) / kItemsInRow,
                                        (axisOffset + i) % kItemsInRow);
    }

    connect(eventThread, &SdlEventThread::axisEvent,
            this, &MainWindow::axisMoved);
    connect(eventThread, &SdlEventThread::hatEvent,
            this, &MainWindow::povPressed);
    connect(eventThread, &SdlEventThread::joyButtonEvent,
        this, &MainWindow::joystickButtonPressed);


    setHaptic(SDL_HapticOpenFromJoystick(joystick));
}

void MainWindow::axisMoved(const SDL_JoyAxisEvent &event)
{
    if (event.which != which)
        return;
    if (event.axis >= numAxis)
        return;
    switch (event.axis) {
    case 0:
        ui->xyaxis->xaxisMoved(event);
        break;
    case 1:
        ui->xyaxis->yaxisMoved(event);
        break;
    case 2:
        ui->zaxis->xaxisMoved(event);
        break;
    case 3:
        ui->zaxis->yaxisMoved(event);
        break;
    case 4:
        ui->axis4->axisMoved(event);
        break;
    case 5:
        ui->axis5->axisMoved(event);
        break;
    }
    QLabel *l = qobject_cast<QLabel*>(
                ui->buttonsLayout->itemAt(numButtons + event.axis)->widget());
    if (!l)
        return;

    if (ui->viewPercent->isChecked())
        l->setText(QString("[%1]: %2%").arg(event.axis).arg((100. * event.value) / SDL_MAX_SINT16, 0, 'f', 1));
    else
        l->setText(QString("[%1]: %2").arg(event.axis).arg(event.value));
}

void MainWindow::povPressed(const SDL_JoyHatEvent &event)
{
    if (event.which != which)
        return;
    if (event.hat != hat)
        return;
    ui->pov->povPressed(event);
}

void MainWindow::setHaptic(struct _SDL_Haptic *haptic)
{
    this->haptic = haptic;
}

void MainWindow::stopHaptic()
{
    if (!haptic) {
        SDL_JoystickRumble(joystick, 0, 0, 0);
        return;
    }
    SDL_HapticStopEffect(haptic, hapticId);
    SDL_HapticDestroyEffect(haptic, hapticId);
    hapticId = -1;
}

void MainWindow::joystickButtonPressed(const SDL_JoyButtonEvent &event)
{
    //qDebug("%s: button %d type %x", __FUNCTION__, event.button, event.type);
    if (event.which != which)
        return;
    if (event.button >= numButtons)
        return;
    QLayoutItem *item = ui->buttonsLayout->itemAt(event.button);
    if (!item) {
        qDebug("%s: no item at %d", __FUNCTION__, event.button);
        return;
    }
    QPushButton *b = qobject_cast<QPushButton*>(item->widget());
    if (!b) {
        qDebug("%s: cast failed at %d", __FUNCTION__, event.button);
        return;
    }

    b->setDown(event.type == SDL_JOYBUTTONDOWN);
    if (!ui->testHaptic->isChecked())
        return;
    if (b->isDown() || hapticId >= 0)
        return;

    SDL_HapticEffect lr = {0};
    lr.type = SDL_HAPTIC_LEFTRIGHT;
    lr.leftright.length = kHapticLength;
    lr.leftright.large_magnitude = (event.button & 1) ? kHapticMagnitude : 0;
    lr.leftright.small_magnitude = (event.button & 1) ? 0 : kHapticMagnitude;

    if (!haptic) {
        if (SDL_JoystickRumble(joystick, lr.leftright.large_magnitude, lr.leftright.small_magnitude, lr.leftright.length) < 0) {
            qWarning("Failed to start rumble: %s\n", SDL_GetError());
            return;
        }
    }

    hapticId = SDL_HapticNewEffect(haptic, &lr);
    if (hapticId < 0) {
        qWarning("Failed to create effect: %s\n", SDL_GetError());
        return;
    }
    //qDebug("[%04x; %04x] : id", lr.leftright.large_magnitude, lr.leftright.small_magnitude, hapticId);
    SDL_HapticRunEffect(haptic, hapticId, 1);
    QTimer::singleShot(kHapticLength + kSafeTimeout, this, &MainWindow::stopHaptic);
}

void MainWindow::joyDeviceAdded(const SDL_JoyDeviceEvent &event)
{
    fillJoystickList();
    if (!joystick)
        setJoystick(event.which);
}

void MainWindow::joyDeviceRemoved(const SDL_JoyDeviceEvent &event)
{
    if (event.which == SDL_JoystickInstanceID(joystick))
        closeJoystick();
    fillJoystickList();
    if (SDL_NumJoysticks() > 0)
        setJoystick(0);
}
