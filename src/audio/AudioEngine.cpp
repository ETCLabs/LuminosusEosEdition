#include "AudioEngine.h"

#include "core/MainController.h"
#include "AudioInputAnalyzer.h"
#include "SpeechInputAnalyzer.h"

#include <QDebug>


AudioEngine::AudioEngine(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
{
    qmlRegisterType<AudioInputAnalyzer>();
    qmlRegisterType<SpeechInputAnalyzer>();
    initInputs();
}

AudioEngine::~AudioEngine() {
    for (AudioInputAnalyzer* inputAnalyzer: m_audioInputs.values()) {
        if (!inputAnalyzer)  continue;
        inputAnalyzer->deleteLater();
    }
    m_audioInputs.clear();
    for (SpeechInputAnalyzer* inputAnalyzer: m_speechInputs.values()) {
        if (!inputAnalyzer)  continue;
        inputAnalyzer->deleteLater();
    }
    m_speechInputs.clear();
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
#ifdef Q_OS_WIN
    return m_audioInputs.first();
#else
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    QString defaultInputName = info.deviceName();
    return getAnalyzerByName(defaultInputName);
#endif
}

AudioInputAnalyzer* AudioEngine::getAnalyzerByName(QString name) const {
    if (m_audioInputs.contains(name)) {
        return m_audioInputs.value(name, nullptr);
    } else {
        for (QString fullName: m_audioInputs.keys()) {
            if (fullName.startsWith(name)) {
                return m_audioInputs.value(fullName, nullptr);
            }
        }
    }
    return nullptr;
}

SpeechInputAnalyzer*AudioEngine::getDefaultSpeechAnalyzer() const {
    if (m_speechInputs.isEmpty()) return nullptr;
#ifdef Q_OS_WIN
    return m_speechInputs.first();
#else
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    QString defaultInputName = info.deviceName();
    return getSpeechAnalyzerByName(defaultInputName);
#endif
}

SpeechInputAnalyzer*AudioEngine::getSpeechAnalyzerByName(QString name) const {
    if (m_speechInputs.contains(name)) {
        return m_speechInputs.value(name, nullptr);
    } else {
        for (QString fullName: m_speechInputs.keys()) {
            if (fullName.startsWith(name)) {
                return m_speechInputs.value(fullName, nullptr);
            }
        }
    }
    return nullptr;
}

double AudioEngine::getMaxLevelOfDevice(QString name) const {
    if (!m_audioInputs.contains(name)) {
        return 0.0;
    }
    AudioInputAnalyzer* inputAnalyzer = m_audioInputs[name];
    if (!inputAnalyzer) return 0.0;
    return inputAnalyzer->getMaxLevel();
}

QStringList AudioEngine::getOutputNameList() const {
    QStringList outputs;
    for (const QAudioDeviceInfo& deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
        if (deviceInfo.deviceName().contains("Generic")) continue;
        outputs << deviceInfo.deviceName();
    }
    return outputs;
}

void AudioEngine::initInputs() {
    // define mor descriptive channel names:
    QMap<int, QString> channelDesc;
    channelDesc[0] = "Left";
    channelDesc[1] = "Right";

    // iterate over all audio input devices:
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);
    for (const QAudioDeviceInfo& device: devices) {
        QString deviceName = device.deviceName();

        // ignore some devices on Linux:
#ifdef Q_OS_LINUX
        if (deviceName.contains("CARD")) continue;
        if (deviceName.contains("default")) continue;
#endif  // Q_OS_LINUX
        if (deviceName == "voicerecognition") continue;


        // check if this device has multiple channels:
        int preferredChannelCount = device.preferredFormat().channelCount();

        // force stereo if available on windows
        // because preferredChannelCount is otherwise always 1
#ifdef Q_OS_WIN
        if (device.supportedChannelCounts().contains(2)) {
            preferredChannelCount = 2;
        }
#endif

        if (preferredChannelCount == 1) {
            // device has only one channel:
            qDebug() << "Audio Input Found:" << deviceName;
            m_audioInputs[deviceName] = new AudioInputAnalyzer(device, 0, deviceName, m_controller);
            m_speechInputs[deviceName] = new SpeechInputAnalyzer(device, 0, deviceName, m_controller);
            QQmlEngine::setObjectOwnership(m_audioInputs[deviceName], QQmlEngine::CppOwnership);
            QQmlEngine::setObjectOwnership(m_speechInputs[deviceName], QQmlEngine::CppOwnership);

        } else if (preferredChannelCount > 1) {
            // device has multiple channels, iterate over them:
            for (int ch=0; ch < preferredChannelCount; ++ch) {
                // create one analyzer per input device channel:
                QString channelName = deviceName + " ";

                // apply some friendly names:
                if (deviceName.contains("BEHRINGER_X18_XR18") && !deviceName.contains("monitor")) {
                    channelName = "Behringer XR 18: USB Out ";
                } else if (deviceName.contains("BEHRINGER_X18_XR18") && deviceName.contains("monitor")) {
                    channelName = "Behringer XR 18: USB Mon ";
                }

                if (preferredChannelCount == 2) {
                    // Stereo Device
                    channelName.append(channelDesc.value(ch, QString::number(ch + 1)));
                } else {
                    // Multichannel Device, i.e. Behringer X32
                    channelName.append(QString("%1").arg(ch + 1, 2, 10, QChar('0')));
                }

                qDebug() << "Audio Input Found:" << channelName;
                m_audioInputs[channelName] = new AudioInputAnalyzer(device, ch, channelName, m_controller);
                m_speechInputs[channelName] = new SpeechInputAnalyzer(device, ch, channelName, m_controller);
                QQmlEngine::setObjectOwnership(m_audioInputs[channelName], QQmlEngine::CppOwnership);
                QQmlEngine::setObjectOwnership(m_speechInputs[channelName], QQmlEngine::CppOwnership);
            }
        }
    }
    // if no devices were found, create a dummy input device:
    if (m_audioInputs.isEmpty()) {
        const QString channelName = "No Input";
        qDebug() << "No audio input found.";
        m_audioInputs[channelName] = new AudioInputAnalyzer(channelName, m_controller);
        m_speechInputs[channelName] = new SpeechInputAnalyzer(channelName, m_controller);
        QQmlEngine::setObjectOwnership(m_audioInputs[channelName], QQmlEngine::CppOwnership);
        QQmlEngine::setObjectOwnership(m_speechInputs[channelName], QQmlEngine::CppOwnership);
    }
}
