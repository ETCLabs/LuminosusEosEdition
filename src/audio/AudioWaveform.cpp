#include "AudioWaveform.h"

#include "core/MainController.h"
#include "QWaveDecoder.h"


namespace AudioWaveformConstants {
    // audio input sampling rate
    static const int AUDIO_SAMPLING_RATE = 44100;  // Hz
}


AudioWaveform::AudioWaveform()
    : QObject(nullptr)
    , m_decoder(nullptr)
    , m_points(1000)
    , m_available(false)
{
}

void AudioWaveform::analyze(QString filename) {
    // reset waveform:
    m_points.fill(0);
    emit pointsChanged();
    m_available = false;
    emit availableChanged();

    if (!filename.toLower().endsWith(".wav")) {
        qInfo() << "AudioWaveform: file is not a .wav file";
        return;
    }
#ifdef Q_OS_WIN
    if (filename.startsWith("file:///")) {
        filename = filename.remove("file:///");
    }
#else
    if (filename.startsWith("file://")) {
        filename = filename.remove("file://");
    }
#endif
    QFile* file = new QFile(filename);
    if (!file->exists()) {
        qInfo() << "AudioWaveform: file doesn't exist: " << filename;
        return;
    }
    file->open(QIODevice::ReadOnly);
    if (!file->isReadable()) {
        qInfo() << "AudioWaveform: couldn't read file: " << filename;
        return;
    }

    // decoder can't be reused,
    // delete old decoder and create a new one:
    delete m_decoder;
    m_decoder = nullptr;
    m_decoder = new QWaveDecoder(file);
    connect(m_decoder, SIGNAL(formatKnown()), this, SLOT(analyzeBuffer()));
}

void AudioWaveform::loadContent(QString filename) {
    if (!filename.toLower().endsWith(".wav")) {
        qInfo() << "AudioWaveform: file is not a .wav file";
        return;
    }
#ifdef Q_OS_WIN
    if (filename.startsWith("file:///")) {
        filename = filename.remove("file:///");
    }
#else
    if (filename.startsWith("file://")) {
        filename = filename.remove("file://");
    }
#endif
    if (filename.startsWith("qrc:")) {
        filename = filename.remove("qrc");
    }
    QFile* file = new QFile(filename);
    if (!file->exists()) {
        qInfo() << "AudioWaveform: file doesn't exist: " << filename;
        return;
    }
    file->open(QIODevice::ReadOnly);
    if (!file->isReadable()) {
        qInfo() << "AudioWaveform: couldn't read file: " << filename;
        return;
    }

    // decoder can't be reused,
    // delete old decoder and create a new one:
    delete m_decoder;
    m_decoder = nullptr;
    m_decoder = new QWaveDecoder(file);
    connect(m_decoder, SIGNAL(formatKnown()), this, SLOT(storeBuffer()));
}

void AudioWaveform::analyzeBuffer() {
    HighResTime::time_point_t begin = HighResTime::now();

    QByteArray data = m_decoder->readAll();
    if (data.isEmpty()) {
        qInfo() << "Wav data is empty";
        return;
    }
    if (data.size() < 1000) {
        qInfo() << "AudioWaveform: Wav file is too short";
        return;
    }

    QAudioFormat format = m_decoder->audioFormat();

    if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt && format.channelCount() == 2) {

        const int16_t* ptr = reinterpret_cast<const int16_t*>(data.constData());

        const int bytesPerFrame = m_decoder->audioFormat().bytesPerFrame();
        const int frames = data.size() / bytesPerFrame;
        const int pointCount = m_points.size();
        const double pointCountD = double(m_points.size() + 1);  // is double for divison

        for (int i=0; i < pointCount; ++i) {
            double startPos = i / pointCountD;
            double endPos = (i+1) / pointCountD;
            int startIndex = int((frames - 1) * startPos) * 2;
            int endIndex = int((frames - 1) * endPos) * 2;
            double rms = 0.0;
            if (endIndex - startIndex < 2) {
                rms = double(*(ptr + startIndex) / 32768);
            } else {
                double sum = 0.0;
                for (int x=startIndex; x<=endIndex; ++x) {
                    const double mono = double(*(ptr + x)) / 32768 + double(*(ptr + x)) / 32768;
                    sum += qPow(mono / 2, 2);
                }
                rms = qPow(sum / (endIndex - startIndex), 0.4);
            }
            m_points[i] = rms;
        }

        m_available = true;
        emit pointsChanged();
        emit availableChanged();

    } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt && format.channelCount() == 1) {

        const int16_t* ptr = reinterpret_cast<const int16_t*>(data.constData());

        const int bytesPerFrame = m_decoder->audioFormat().bytesPerFrame();
        const int frames = data.size() / bytesPerFrame;
        const int pointCount = m_points.size();
        const double pointCountD = double(m_points.size() + 1);  // is double for divison

        for (int i=0; i < pointCount; ++i) {
            double startPos = i / pointCountD;
            double endPos = (i+1) / pointCountD;
            int startIndex = int((frames - 1) * startPos);
            int endIndex = int((frames - 1) * endPos);
            double rms = 0.0;
            if (endIndex - startIndex < 2) {
                rms = double(*(ptr + startIndex) / 32768);
            } else {
                double sum = 0.0;
                for (int x=startIndex; x<=endIndex; ++x) {
                    sum += qPow(double(*(ptr + x)) / 32768, 2);
                }
                rms = qPow(sum / (endIndex - startIndex), 0.4);
            }
            m_points[i] = rms;
        }

        m_available = true;
        emit pointsChanged();
        emit availableChanged();

    } else {
        qInfo() << "AudioWaveform: Wav file is not saved as signed 16bit mono / stereo.";
    }

    qDebug() << "Time to analyze wav: " << HighResTime::elapsedSecSince(begin);
}

void AudioWaveform::storeBuffer() {
    m_rawData = m_decoder->readAll();
    emit contentLoaded();
}
