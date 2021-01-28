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

const int kHapticLength = 500;
const int kSafeTimeout  = 50;

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

    connect(eventThread, &SdlEventThread::axisEvent,
            this, &MainWindow::axisMoved);
    connect(eventThread, &SdlEventThread::joyButtonEvent,
        this, &MainWindow::joystickButtonPressed);
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

    while (auto child = ui->buttonsLayout->takeAt(0)) {
        delete child;
    }

    joystick = 0;
    numAxis    = 0;
    numButtons = 0;
    numHats    = 0;
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
        b->setCheckable(true);
        ui->buttonsLayout->addWidget(b, i / 6, i % 6);
    }
}

void MainWindow::setAxis(SdlAxisWidget *widget, int xaxis, int yaxis)
{
    if (std::max(xaxis, yaxis) < numAxis) {
        widget->init(joystick, xaxis, yaxis);
        connect(eventThread, &SdlEventThread::axisEvent,
            widget, &SdlAxisWidget::axisMoved);
    } else {
        disconnect(eventThread, &SdlEventThread::axisEvent,
            widget, &SdlAxisWidget::axisMoved);
        widget->reset();
    }
}

void MainWindow::setPov(SdlPovWidget *widget, int hat)
{
    if (hat < numHats) {
        widget->init(joystick, hat);
        connect(eventThread, &SdlEventThread::hatEvent,
            widget, &SdlPovWidget::povPressed);
    } else {
        disconnect(eventThread, &SdlEventThread::hatEvent,
            widget, &SdlPovWidget::povPressed);
        widget->reset();
    }
}

void MainWindow::setSlider(SdlSliderWidget *widget, int axis)
{
    if (axis < numAxis) {
        widget->init(joystick, axis);
        connect(eventThread, &SdlEventThread::axisEvent,
            widget, &SdlSliderWidget::axisMoved);
    } else {
        disconnect(eventThread, &SdlEventThread::axisEvent,
            widget, &SdlSliderWidget::axisMoved);
        widget->reset();
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

    ui->choosePadBox->setCurrentIndex(index + 1);

    qDebug("%s: setup joystick %d %s", __CLASS__,
           SDL_JoystickInstanceID(joystick), SDL_JoystickName(joystick));

    numAxis    = SDL_JoystickNumAxes(joystick);
    numButtons = SDL_JoystickNumButtons(joystick);
    numHats    = SDL_JoystickNumHats(joystick);

    setPov   (ui->pov,    0);
    setAxis  (ui->xyaxis, 0, 1);
    setAxis  (ui->zaxis,  2, 3);
    setSlider(ui->axis4,  4);
    setSlider(ui->axis5,  5);
    setButtons(numButtons);

    int axisOffset = numButtons;
    while (axisOffset % 6) ++axisOffset;
    for (int i = 0; i<numAxis; i++) {
        QLabel *l = new QLabel(this);
        l->setText(QString("[%1]").arg(i));
        ui->buttonsLayout->addWidget(l, (axisOffset + i) / 6,
                                        (axisOffset + i) % 6);
    }

    setHaptic(SDL_HapticOpenFromJoystick(joystick));
}

void MainWindow::axisMoved(SDL_JoyAxisEvent event)
{
    if (event.axis >= numAxis)
        return;
    QLabel *l = qobject_cast<QLabel*>(
                ui->buttonsLayout->itemAt(numButtons + event.axis)->widget());
    if (!l)
        return;
    l->setText(QString("[%1]: %2").arg(event.axis).arg(event.value));
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

void MainWindow::joystickButtonPressed(SDL_JoyButtonEvent event)
{
    //qDebug("%s: button %d type %x", __FUNCTION__, event.button, event.type);
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

    b->setChecked(event.type == SDL_JOYBUTTONDOWN);
    if (!ui->testHaptic->isChecked())
        return;
    if (b->isChecked() || hapticId >= 0)
        return;

    SDL_HapticEffect lr = {0};
    lr.type = SDL_HAPTIC_LEFTRIGHT;
    lr.leftright.length = kHapticLength;
    lr.leftright.large_magnitude = (event.button & 1) ? 0x7FFF : 0;
    lr.leftright.small_magnitude = (event.button & 1) ? 0 : 0x7FFF;

    if (!haptic) {
        if (SDL_JoystickRumble(joystick, lr.leftright.large_magnitude, lr.leftright.small_magnitude, lr.leftright.length) < 0)
            qWarning("Failed to start rumble: %s\n", SDL_GetError());
            return;
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

void MainWindow::joyDeviceAdded(SDL_JoyDeviceEvent event)
{
    if (!joystick)
        setJoystick(event.which);
}

void MainWindow::joyDeviceRemoved(SDL_JoyDeviceEvent event)
{
    if (event.which == SDL_JoystickInstanceID(joystick))
        closeJoystick();
    if (SDL_NumJoysticks() > 0)
        setJoystick(0);
}
