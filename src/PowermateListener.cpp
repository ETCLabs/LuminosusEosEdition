// Copyright (c) 2016 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "PowermateListener.h"
#include "utils.h"

#if defined(Q_OS_LINUX)
    #include <linux/input.h>
#endif

#if defined(Q_OS_LINUX)  // LINUX

PowermateListener::PowermateListener(QObject *parent) : QThread(parent), device_out("/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00") {
    if(!device_out.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        if (device_out.error() != QFileDevice::OpenError) {
            // QFileDevice::OpenError often means PowerMate is not connected -> not interesting
            qDebug() << "Error opening powermate device: " << device_out.error();
        }
        return;
    }
}

void PowermateListener::processEvent(const input_event &event)
{
    if (event.type == EV_REL) {
        if(event.code != REL_DIAL) {
            qDebug() << "Powermate: Unexpected rotation event: " << event.code;
        } else {
            // time since last rotation event:
            double time = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
            double delta = time - lastRotationTime;
            lastRotationTime = time;

            double acceleration = limit(1., 25 / delta, 4.);
            double rotation = (event.value * acceleration) / 512.;
            emit rotated(rotation, isPressed);
        }
    } else if (event.type == EV_KEY) {
        if(event.code != BTN_0) {
            qDebug() << "Powermate: Unexpected button " << event.code;
        } else {
            if(event.value) {
                isPressed = true;
                lastPressTime = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
                emit pressed();
            } else {
                isPressed = false;
                // time since last press event:
                double time = event.time.tv_sec * 1000 + (event.time.tv_usec * 0.001);
                double deltaInSec = (time - lastPressTime) / 1000;
                emit released(deltaInSec);
            }
        }
    }
}

void PowermateListener::run() {
    QFile device("/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00");
    if(!device.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        if (device_out.error() != QFileDevice::OpenError) {
            // QFileDevice::OpenError often means PowerMate is not connected -> not interesting
            qDebug() << "Error opening powermate device: " << device_out.error();
        }
        return;
    }
    struct input_event event;
    while (true) {
        int bytes_read = device.read((char*)&event, sizeof(struct input_event));
        if (bytes_read == 0) break;
        if (bytes_read != sizeof(struct input_event)) {
            qDebug() << "Powermate: read only " << bytes_read << "bytes, expected " << sizeof(struct input_event);
        } else {
            processEvent(event);
        }
    }
}

void PowermateListener::setBrightness(double value) {
    int brightness = int(limit(0., value*255, 255.));
    input_event event;
    event.time = timeval{0, 0};
    event.type = 0x04;
    event.code = 0x01;
    event.value = brightness;
    device_out.write((char*)&event, sizeof(struct input_event));
}

void PowermateListener::startPulsing() {
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
    device_out.write((char*)&event, sizeof(struct input_event));
}

#endif // LINUX

#if !defined(Q_OS_LINUX)  // other OS

PowermateListener::PowermateListener(QObject *parent) : QThread(parent) { }

void PowermateListener::run() { }

void PowermateListener::setBrightness(double) { }
void PowermateListener::startPulsing() { }

#endif  // other OS
