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

#include "AudioEngine.h"

#include "MainController.h"

#include <QDebug>


AudioEngine::AudioEngine(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
{
    qmlRegisterType<AudioInputAnalyzer>();
    initInputs();
}

AudioEngine::~AudioEngine() {
    for (AudioInputAnalyzer* inputAnalyzer: m_audioInputs.values()) {
        if (!inputAnalyzer)  continue;
        inputAnalyzer->deleteLater();
    }
    m_audioInputs.clear();
}

QStringList AudioEngine::getDeviceNameList() const {
    QStringList list;
    for (QString inputName: m_audioInputs.keys()) {
        list.append(inputName);
    }
    return list;
}

AudioInputAnalyzer* AudioEngine::getDefaultAnalyzer() const {
    if (m_audioInputs.isEmpty()) return nullptr;
    QString defaultInputName = QAudioDeviceInfo::defaultInputDevice().deviceName();
    return getAnalyzerByName(defaultInputName);
}

AudioInputAnalyzer* AudioEngine::getAnalyzerByName(QString name) const {
    return m_audioInputs.value(name, nullptr);
}

double AudioEngine::getMaxLevelOfDevice(QString name) const {
    if (!m_audioInputs.contains(name)) {
        return 0.0;
    }
    AudioInputAnalyzer* inputAnalyzer = m_audioInputs[name];
    if (!inputAnalyzer) return 0.0;
    return inputAnalyzer->getMaxLevel();
}

void AudioEngine::initInputs() {
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);
    for (const QAudioDeviceInfo& device: devices) {
        qDebug() << "Audio Input Found:" << device.deviceName();
#ifdef Q_OS_LINUX
        if (device.deviceName().contains("CARD")) continue;
        if (device.deviceName().contains("default")) continue;
#endif  // Q_OS_LINUX
        m_audioInputs[device.deviceName()] = new AudioInputAnalyzer(device, m_controller);
        QQmlEngine::setObjectOwnership(m_audioInputs[device.deviceName()], QQmlEngine::CppOwnership);
    }
    // if no devices were found, create a dummy input device:
    if (m_audioInputs.isEmpty()) {
        m_audioInputs["No Input"] = new AudioInputAnalyzer(m_controller);
        QQmlEngine::setObjectOwnership(m_audioInputs["No Input"], QQmlEngine::CppOwnership);
    }
}
