#include "PowermateListener.h"

#include "utils.h"

#include <QDebug>

#if defined(Q_OS_LINUX)  // LINUX

namespace PowermateListenerConstants {
    static const QString typicalDeviceName = "/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00";

    static const double degreesPerTick = 3.6;  // 360 degree / 100 ticks
}


PowermateListener::PowermateListener(QObject* parent)
    : QThread(parent)
    , m_isPressed(false)
    , m_lastPressTime(0.0)
    , m_lastRotationTime(0.0)
    , m_outputDevice(PowermateListenerConstants::typicalDeviceName)
{
    if (!m_outputDevice.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        if (m_outputDevice.error() != QFileDevice::OpenError) {
            // QFileDevice::OpenError often means PowerMate is not connected -> not interesting
            qDebug() << "Error opening powermate device for writing: " << m_outputDevice.error();
        }
        return;
    }
}

void PowermateListener::processEvent(const input_event& event) {
    if (event.type == EV_REL) {
        if (event.code != REL_DIAL) {
            qDebug() << "Powermate: Unexpected rotation event: " << event.code;
        } else {
            // time since last rotation event:
            double time = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
            double delta = time - m_lastRotationTime;
            m_lastRotationTime = time;

            double acceleration = limit(1., 25 / delta, 4.);
            double rotation = (event.value * acceleration) / 512.;
            emit rotated(rotation, event.value * PowermateListenerConstants::degreesPerTick, m_isPressed);
        }
    } else if (event.type == EV_KEY) {
        if (event.code != BTN_0) {
            qDebug() << "Powermate: Unexpected button " << event.code;
        } else {
            if(event.value) {
                m_isPressed = true;
                m_lastPressTime = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
                emit pressed();
            } else {
                m_isPressed = false;
                // time since last press event:
                double time = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
                double deltaInSec = (time - m_lastPressTime) / 1000;
                emit released(deltaInSec);
            }
        }
    }
}

void PowermateListener::run() {
    QFile inputDevice(PowermateListenerConstants::typicalDeviceName);
    if (!inputDevice.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        if (m_outputDevice.error() == QFileDevice::OpenError) {
            // QFileDevice::OpenError often means PowerMate is not connected -> not interesting
            if (m_outputDevice.errorString() == "Permission denied") {
                qInfo() << "Powermate was found, but access was denied.";
                qInfo() << "Try and reboot: sudo addgroup $USER input; echo 'ATTRS{product}==\"Griffin PowerMate\" GROUP=\"input\", MODE=\"0666\"' | sudo tee /etc/udev/rules.d/99-input.rules";
            } else {
                qInfo() << "No Powermate device found:" << m_outputDevice.errorString();
            }
        } else {
            qWarning() << "Error opening powermate device: " << m_outputDevice.error();
        }
        emit connectedChanged();
        return;
    }
    emit connectedChanged();
    struct input_event event;
    while (true) {
        int bytes_read = inputDevice.read((char*)&event, sizeof(struct input_event));
        if (bytes_read == 0) break;
        if (bytes_read != sizeof(struct input_event)) {
            qDebug() << "Powermate: read only " << bytes_read << "bytes, expected " << sizeof(struct input_event);
        } else {
            processEvent(event);
        }
    }
}

void PowermateListener::setBrightness(double value) {
    if (!m_outputDevice.isWritable()) return;
    int brightness = int(limit(0., value*255, 255.));
    input_event event;
    event.time = timeval{0, 0};
    event.type = 0x04;
    event.code = 0x01;
    event.value = brightness;
    m_outputDevice.write((char*)&event, sizeof(struct input_event));
}

void PowermateListener::startPulsing() {
    if (!m_outputDevice.isWritable()) return;
    int brightness = 255;
    int speed = 255;
    int pulseType = speed > 0 ? 2 : 2;
    int asleep = speed > 0 ? 1 : 1;
    int awake = speed > 0 ? 1 : 1;
    int32_t data = brightness |
            (speed << 8) |
            (pulseType << 17) |
            (asleep << 19) |
            (awake << 20);
    input_event event;
    event.time = timeval{0, 0};
    event.type = 0x04;
    event.code = 0x01;
    event.value = data;
    m_outputDevice.write((char*)&event, sizeof(struct input_event));
}

bool PowermateListener::connected() const {
    return m_outputDevice.isReadable();
}

#endif // LINUX

#if !defined(Q_OS_LINUX)  // other OS

PowermateListener::PowermateListener(QObject *parent) : QThread(parent) { }

void PowermateListener::run() { }

void PowermateListener::setBrightness(double) { }
void PowermateListener::startPulsing() { }

bool PowermateListener::connected() const { return false; }

#endif  // other OS
