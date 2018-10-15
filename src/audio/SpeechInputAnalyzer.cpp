#include "SpeechInputAnalyzer.h"

#include "core/MainController.h"

#include <QtMath>
#include <QDebug>


// --------- Constants for Spectrum Creation -----------

// audio input sampling rate
static const int AUDIO_SAMPLING_RATE = 16000;  // Hz

// length of circular input buffer in samples
static const int CIRC_BUFFER_LENGTH = AUDIO_SAMPLING_RATE * 0.3;  // 0.3s

// number of samples to use for the "short" mid-high spectrum
static const int SHORT_NUM_SAMPLES = qPow(2, SPEECH_FFT_SAMPLES_EXPONENT);  // 9 -> 512

// number of data points for the simplified spectrum
static const int SIMPLIFIED_SPECTRUM_LENGTH = 128;

// number of simplified spectrums to keep in the history
static const int SPECTRUM_HISTORY_LENGTH = 10;

// --------- Constants for Speech Recording -----------

// number of samples per audio part for streaming speech recognition
static const int STREAMING_RECOGNITION_PART_LENGTH = 5000;  // Samples

// --------- Spectral Flux -----------

// rate to calculate the spectrum when not recording
static const int IDLE_SPECTRUM_UPDATE_RATE = 10;  // Hz

// rate to calculate the spectrum and spectral flux when recording
static const int SPECTRUM_UPDATE_RATE = 20;  // Hz

// number of spectral flux values to keep in the history
static const int SPECTRAL_FLUX_HISTORY_LENGTH = SPECTRUM_UPDATE_RATE * 5;  // 5s

// maximum absolute value in the result of the FFT
// estimated from previous tests (exponent/samples: max value):
// 11/2048: 51, 12/4096: 96, 13/8192: 195, 14/16384: 350
static const int SHORT_MAX_FFT_VALUE = 1;

// ----------------- AGC Constants -----------------

// AGC = Automatic Gain Control

// number of samples to average for the AGC calculation
static const int AGC_AVERAGING_LENGTH = SPECTRUM_UPDATE_RATE*2;  // samples

// headroom to leave when using AGC [0...1]
static const float AGC_HEADROOM = 0.1;  // 10%

// min value for AGC to be active == max value of noise [0...1]
static const float AGC_NOISE_THRESHOLD = 0.1; // 10%

// amount to increase the gain per frame when gain is too low
// i.e. 1 / <frames needed to increase the gain from 0 to 1>
static const float AGC_INCREMENT_STEPSIZE = 1.0 / (3*SPECTRUM_UPDATE_RATE);  // 3s * SPECTRUM_UPDATE_RATE

// amount to decrease the gain per frame when gain is too high
// i.e. 1 / <frames needed to decrease the gain from 1 to 0>
static const float AGC_DECREMENT_STEPSIZE = 1.0 / (1*SPECTRUM_UPDATE_RATE);  // 1s * SPECTRUM_UPDATE_RATE

// minimum gain of AGC
static const float AGC_MIN_GAIN = 0.5;

// maximum gain of AGC
static const float AGC_MAX_GAIN = 5.0;


// ------------------------------- Utility Functions for BPM Detection -------------------------------

inline int frequencyToIndex(const int frequency) {
    return SHORT_NUM_SAMPLES * frequency / AUDIO_SAMPLING_RATE;
}


// ---------------------------------- SpeechInputAnalyzer --------------------------------

SpeechInputAnalyzer::SpeechInputAnalyzer(QAudioDeviceInfo inputInfo, int channelIndex, QString name, MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_referenceList()
    , m_audioInput(nullptr)
    , m_audioCaptureIODevice(nullptr)
    , m_deviceInfo(inputInfo)
    , m_deviceName(name)
    , m_channelIndex(channelIndex)
    , m_isRecording(false)
    , m_circBuffer(CIRC_BUFFER_LENGTH)
    , m_shortBuffer(SHORT_NUM_SAMPLES)
    , m_shortWindow(SHORT_NUM_SAMPLES)
    , m_lastShortFftOutput(SHORT_NUM_SAMPLES)
    , m_shortFftOutput(SHORT_NUM_SAMPLES)
    , m_shortSpectrum(SHORT_NUM_SAMPLES / 2)
    , m_spectrumHistory(SPECTRUM_HISTORY_LENGTH)
    , m_dummySpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    , m_currentRMS(0.0)
    , m_maxLevel(0.0)
    , m_agcValue(1.0)
    , m_spectralFluxAgcValue(1.0)
    , m_compression(1.0)
    , m_lastMaxValues(AGC_AVERAGING_LENGTH)
    , m_lastRmsValues(32)
    , m_spectralFluxHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralColorHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralFluxNormalized(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_currentSpectralFlux(0.0)
{
    // fill circular buffer with zeros:
    for (int i=0; i < m_circBuffer.capacity(); ++i) {
        m_circBuffer.push_back(0.0);
    }
    m_circBuffer.fill(0.0, m_circBuffer.capacity());
    m_spectralFluxHistory.fill(0.0, m_spectralFluxHistory.capacity());
    m_spectralColorHistory.fill(0.0, m_spectralColorHistory.capacity());
    calculateWindows();

    m_fftTimer.setSingleShot(false);
    m_fftTimer.setInterval(1000.0 / IDLE_SPECTRUM_UPDATE_RATE);
    connect(&m_fftTimer, SIGNAL(timeout()), this, SLOT(analyzeAudio()));
}

SpeechInputAnalyzer::~SpeechInputAnalyzer() {
    if (!m_audioInput.isNull()) {
        m_audioInput->stop();
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }
}

SpeechInputAnalyzer::SpeechInputAnalyzer(QString name, MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_referenceList()
    , m_audioInput(nullptr)
    , m_audioCaptureIODevice(nullptr)
    , m_deviceName(name)
    , m_channelIndex(0)
    , m_isRecording(false)
    , m_circBuffer(0)
    , m_spectrumHistory(SPECTRUM_HISTORY_LENGTH)
    , m_dummySpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    , m_currentRMS(0.0)
    , m_maxLevel(0.0)
    , m_agcValue(1.0)
    , m_spectralFluxAgcValue(1.0)
    , m_compression(1.0)
    , m_lastMaxValues(AGC_AVERAGING_LENGTH)
    , m_spectralFluxHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralColorHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralFluxNormalized(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_currentSpectralFlux(0.0)
{

}

void SpeechInputAnalyzer::addReference(void* ref) {
    if (m_referenceList.isEmpty()) {
        // this is the first registered object
        createAudioInputForSpeech();
        startAudioCapture();
        m_spectralFluxHistory.fill(0.0, m_spectralFluxHistory.capacity());
        m_spectralColorHistory.fill(0.0, m_spectralColorHistory.capacity());
        m_fftTimer.start();
    }
    m_referenceList.insert(ref);
}

void SpeechInputAnalyzer::removeReference(void* ref) {
    m_referenceList.remove(ref);
    if (m_referenceList.isEmpty()) {
        // this was the last registered object
        stopAudioCapture();
        destroyAudioInputForSpeech();
        m_fftTimer.stop();
    }
}

bool SpeechInputAnalyzer::isCapturing() const {
    return m_audioCaptureIODevice != nullptr;
}

void SpeechInputAnalyzer::startRecording() {
    if (m_isRecording) {
        qWarning() << "Already recording.";
        return;
    }

    m_speechRecording.clear();
    m_speechRecordingPart.clear();
    m_lastRmsValues.fill(0.0, m_lastRmsValues.capacity());

    m_isRecording = true;
    addReference(this);
    m_fftTimer.setInterval(1000.0 / SPECTRUM_UPDATE_RATE);
    emit isRecordingChanged();
}

const QByteArray& SpeechInputAnalyzer::stopRecording() {
    if (!m_isRecording) {
        qWarning() << "Not recording.";
        return m_speechRecording;
    }
    removeReference(this);
    m_isRecording = false;
    m_fftTimer.setInterval(1000.0 / IDLE_SPECTRUM_UPDATE_RATE);
    emit isRecordingChanged();
    return m_speechRecording;
}

const std::vector<double>&SpeechInputAnalyzer::getSimplifiedSpectrum() const {
    if (!m_spectrumHistory.isEmpty()) {
        return m_spectrumHistory.last();
    } else {
        return m_dummySpectrum;
    }
}

QVector<double> SpeechInputAnalyzer::getValueHistory() const {
    int numValues = m_lastRmsValues.size();
    QVector<double> values(numValues);
    for (int i=0; i < numValues; ++i) {
        values[i] = m_lastRmsValues[i] * m_agcValue;
    }
    return values;
}

void SpeechInputAnalyzer::calculateWindows() {
    // Hann Window function
    for (int i=0; i<SHORT_NUM_SAMPLES; ++i) {
        m_shortWindow[i] = 0.5f * (1 - qCos((2 * M_PI * i) / (SHORT_NUM_SAMPLES - 1)));
    }
}

void SpeechInputAnalyzer::createAudioInputForSpeech() {
    if (m_audioInput) {
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }

    // Set up the desired audio input format:
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // check if the format is support:
    if (!m_deviceInfo.isFormatSupported(format)) {
        // it is not -> use the nearest supported one:
        qWarning() << "Default audio format not supported, trying to use the nearest.";
        format = m_deviceInfo.nearestFormat(format);
        if (format.sampleRate() != 16000) {
            qCritical() << "Could not open audio device with 16kHz sample rate.";
            return;
        }
        if (format.sampleSize() != 16) {
            qCritical() << "Could not open audio device with 16bit sample size.";
            return;
        }
    }
    m_audioFormat = format;

    // create a QAudioInput object:
    m_audioInput = new QAudioInput(m_deviceInfo, format);
    if (!m_audioInput) {
        qCritical() << "Could not open audio device.";
        return;
    }
}

void SpeechInputAnalyzer::destroyAudioInputForSpeech() {
    if (m_audioInput) {
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }
}

void SpeechInputAnalyzer::startAudioCapture() {
    if (!m_audioInput) {
        qCritical() << "Can't start audio capture: no audio input device.";
        return;
    }
    if (!m_audioCaptureIODevice) {
        m_circBuffer.fill(0.0, m_circBuffer.capacity());
        m_audioCaptureIODevice = m_audioInput->start();
        if (!m_audioCaptureIODevice) {
            qCritical() << "Could not start audio capture.";
            qCritical() << m_audioInput->error();
            return;
        }
        if (m_audioInput->volume() < 1.0) m_audioInput->setVolume(1.0);
        connect(m_audioCaptureIODevice, SIGNAL(readyRead()), this, SLOT(onAudioDataReady()));
        emit isCapturingChanged();
    }
}

void SpeechInputAnalyzer::stopAudioCapture() {
    if (m_audioCaptureIODevice) {
        disconnect(m_audioCaptureIODevice, SIGNAL(readyRead()), this, SLOT(onAudioDataReady()));
        m_audioCaptureIODevice = nullptr;  // is deleted by QAudioInput
        emit isCapturingChanged();
    }
    if (m_audioInput) {
        m_audioInput->stop();
    }
}

void SpeechInputAnalyzer::onAudioDataReady() {
    if (!m_audioCaptureIODevice) return;

    const QByteArray data = m_audioCaptureIODevice->readAll();
    const int bytesPerSample = m_audioFormat.bytesPerFrame();
    const int numSamples = data.size() / bytesPerSample;
    if (numSamples <= 0) return;

    // Speech Recording:
    if (m_isRecording) {
        m_speechRecording.append(data);
        m_speechRecordingPart.append(data);
        if (m_speechRecordingPart.size() > STREAMING_RECOGNITION_PART_LENGTH) {
            emit speechPartReady(m_speechRecordingPart);
            m_speechRecordingPart.clear();
        }
    }

    const char* ptr = data.constData();
    ptr += m_audioFormat.sampleSize() * m_channelIndex / 8;
    float squaredSum = 0.0;

    // write input data to circular buffer:
    // (assuming 16 bit sample size)
    for (int i=0; i<numSamples; ++i) {
        // interpret byte in QByteArray as int16:
        const int16_t pcmSample = *reinterpret_cast<const int16_t*>(ptr);
        // convert to float and scale down to range [-1.0, 1.0]:
        const float scaled = float(pcmSample) / 32768;
        squaredSum += pow(scaled, 2);
        // push to circular buffer:
        m_circBuffer.push_back(scaled);
        ptr += bytesPerSample;
    }

    m_currentRMS = std::sqrt(squaredSum / numSamples);
    m_lastRmsValues.push_back(m_currentRMS);
    emit currentLevelChanged();
}

void SpeechInputAnalyzer::analyzeAudio() {
    if (m_currentRMS < 0.004) {
        // 0.004 is background noise of microphone
        // if RMS is below that don't calculate FFT and clear previous spectrum
        if (m_spectrumHistory.isEmpty() || !qFuzzyCompare(m_spectrumHistory.last()[0], 0)) {
            m_spectrumHistory.push_back(m_dummySpectrum);
            emit spectrumChanged();
        }
        return;
    }
    if (m_isRecording) {
        calculateRawSpectrum();
    } else {
        calculateRawSpectrumWithoutFlux();
    }
    createSimplifiedSpectrumFromRawSpectrum();
    emit spectrumChanged();
}

void SpeechInputAnalyzer::calculateRawSpectrum() {
    // copy data from circular buffer to m_shortBuffer and apply window:
    const int startIndex = m_circBuffer.size() - SHORT_NUM_SAMPLES;
    for (int i=0; i < SHORT_NUM_SAMPLES; ++i) {
        m_shortBuffer[i] = m_circBuffer[startIndex + i] * m_shortWindow[i];
    }
    // apply FFT to m_shortBuffer and write result to m_shortFftOutput:
    m_shortFftreal.do_fft(m_shortFftOutput.data(), m_shortBuffer.data());

    float flux = 0.0;
    float maxMagnitude = 0.0;  // used for AGC

    // iterate over FFT result:
    for (int i=0; i < SHORT_NUM_SAMPLES / 2; ++i) {
        // calculate magnitude for each FFT bin:
        const float real = m_shortFftOutput[i];
        const float img = m_shortFftOutput[SHORT_NUM_SAMPLES / 2 + i];
        //const float magnitudeInDb = 4.342f * std::log10(real*real + img*img);  // simplified from 20*log10(sqrt(r*r+i*i)) -> dB of Magnitude
        //const float magnitude = std::pow(real*real + img*img, 0.2) / 9;  // 9 is most of the time the max value
        const float magnitude = std::pow(real*real + img*img, 0.3) / 32;  // 32 is most of the time the max value
        // check if this is the current maximum value:
        maxMagnitude = std::max(maxMagnitude, magnitude);
        // write normalized magnitude to m_shortSpectrum:
        m_shortSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));

        // Calculates the spectral flux for the samples from the given index
        // Spectral flux is the sum of only the *increases* in frequency.
        // See "Evaluation of the Audio Beat Tracking System BeatRoot" by Simon Dixon
        // (in Journal of New Music Research, 36, 2007/8) for further detail
        if (m_shortFftOutput[i] > m_lastShortFftOutput[i]) {
            flux += m_shortFftOutput[i] - m_lastShortFftOutput[i];
        }
    }
    m_maxLevel = limit(0.0f, maxMagnitude, 1.0f);
    m_spectralFluxHistory.push_back(flux);
    m_lastShortFftOutput = m_shortFftOutput;

    // ----- Automatic Gain Control:
    m_lastMaxValues.push_back(maxMagnitude);
    updateAGC();
}

void SpeechInputAnalyzer::calculateRawSpectrumWithoutFlux() {
    // copy data from circular buffer to m_shortBuffer and apply window:
    const int startIndex = m_circBuffer.size() - SHORT_NUM_SAMPLES;
    for (int i=0; i < SHORT_NUM_SAMPLES; ++i) {
        m_shortBuffer[i] = m_circBuffer[startIndex + i] * m_shortWindow[i];
    }
    // apply FFT to m_shortBuffer and write result to m_shortFftOutput:
    m_shortFftreal.do_fft(m_shortFftOutput.data(), m_shortBuffer.data());

    float maxMagnitude = 0.0;  // used for AGC

    // iterate over FFT result:
    for (int i=0; i < SHORT_NUM_SAMPLES / 2; ++i) {
        // calculate magnitude for each FFT bin:
        const float real = m_shortFftOutput[i];
        const float img = m_shortFftOutput[SHORT_NUM_SAMPLES / 2 + i];
        const float magnitude = std::pow(real*real + img*img, 0.3) / 32;  // 32 is most of the time the max value
        // check if this is the current maximum value:
        maxMagnitude = std::max(maxMagnitude, magnitude);
        // write normalized magnitude to m_shortSpectrum:
        m_shortSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));
    }
    m_maxLevel = limit(0.0f, maxMagnitude, 1.0f);
    // don't copy due to performance reasons:
    //m_lastShortFftOutput = m_shortFftOutput;

    // ----- Automatic Gain Control:
    m_lastMaxValues.push_back(maxMagnitude);
    updateAGCWithoutFlux();
}

void SpeechInputAnalyzer::createSimplifiedSpectrumFromRawSpectrum() {
    const int beginFreq = 10;
    const double factor = 1.05361122898671;  // pow(8000 / 10., 1/128.)
    //const double factor = 1.0305289337977435;  // pow(22050 / 10., 1/256.)
    //const float maxPossibleEnergy = (MAX_FFT_VALUE*qMax(std::size_t(1), valuesTillNext));
    //const float longMaxPossibleEnergy = LONG_MAX_FFT_VALUE;
    //const float shortMaxPossibleEnergy = SHORT_MAX_FFT_VALUE;

    std::vector<double> simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH);
    double freq = beginFreq;

    for (int i=0; i<SIMPLIFIED_SPECTRUM_LENGTH; ++i) {
        const double nextFreq = beginFreq * std::pow(factor, i+1);
        const int startIndex = freq / 8000 * (SHORT_NUM_SAMPLES / 2);
        const int endIndex = nextFreq / 8000 * (SHORT_NUM_SAMPLES / 2);
        const int valuesTillNext = endIndex - startIndex;
        if (endIndex >= SHORT_NUM_SAMPLES) continue;

        // find max value in this range:
        float max = m_shortSpectrum[startIndex];
        for (int j=1; j < valuesTillNext; ++j) {
            max = std::max(max, m_shortSpectrum[startIndex+j]);
        }
        simplifiedSpectrum[i] = max;

//            // Sum up the energies of all FFT elements between the two frequencies:
//            float sumEnergy = m_shortSpectrum[startIndex];
//            for (int j=1; j < valuesTillNext; ++j) {
//                sumEnergy += m_shortSpectrum[startIndex+j];
//            }

//            // Apply reasonable scale:
//            sumEnergy /= shortMaxPossibleEnergy;

//            // Scale the value with factor and exponent:
//            simplifiedSpectrum[i] = qPow(qMax(0.0f, qMin(sumEnergy, 1.0f)), (1 / m_compression));
        freq = nextFreq;
    }

    // ----------------------- Calculate Spectral Color -----------------------

    /*
    if (m_detectBpm) {
        double redMax = 0;
        double greenMax = 0;
        double blueMax = 0;
        for (int i=0; i < (SIMPLIFIED_SPECTRUM_LENGTH * 0.3); ++i) {
            redMax = qMax(redMax, simplifiedSpectrum[i]);
        }
        for (int i=(SIMPLIFIED_SPECTRUM_LENGTH * 0.3); i < (SIMPLIFIED_SPECTRUM_LENGTH * 0.6); ++i) {
            greenMax = qMax(greenMax, simplifiedSpectrum[i]);
        }
        for (int i=(SIMPLIFIED_SPECTRUM_LENGTH * 0.6); i < SIMPLIFIED_SPECTRUM_LENGTH; ++i) {
            blueMax = qMax(blueMax, simplifiedSpectrum[i]);
        }
        const double maxComponent = qMax(redMax, qMax(greenMax, blueMax));
        if (maxComponent == 0) {
            m_spectralColorHistory.push_back(QColor::fromRgbF(1, 1, 1));
        } else {
            m_spectralColorHistory.push_back(
                     QColor::fromRgbF(redMax / maxComponent, greenMax / maxComponent, blueMax / maxComponent));
        }
    }
    */

    // push spectrum to spectrum history with move semantic:
    m_spectrumHistory.push_back(move(simplifiedSpectrum));
    // Attention: because of move semantic simplifiedSpectrum is now unusable!
    emit spectrumChanged();
}

void SpeechInputAnalyzer::updateAGC() {
    // spectrum AGC:
    {
        // get max of last values:
        float maxValue = 0.0;
        for (int i=0; i<m_lastMaxValues.size(); ++i) {
            maxValue = qMax(maxValue, m_lastMaxValues[i]);
        }

        // check if maxValue is below noise threshold:
        if (maxValue < AGC_NOISE_THRESHOLD || maxValue <= 0) {
            // do not change current gain
            return;
        }

        // calculate required gain:
        const float requiredGain = (1 - AGC_HEADROOM) / maxValue;

        // adjust gain in small steps:
        if (requiredGain < m_agcValue) {
            m_agcValue = qMax(AGC_MIN_GAIN, qMax(requiredGain, m_agcValue - AGC_DECREMENT_STEPSIZE));
        } else {
            m_agcValue = qMin(AGC_MAX_GAIN, qMin(requiredGain, m_agcValue + AGC_INCREMENT_STEPSIZE));
        }
    }
    // spectral flux AGC:
    {
        // get max of last values:
        float maxValue = 0.0;
        for (int i=m_spectralFluxHistory.size() - m_lastMaxValues.size(); i<m_spectralFluxHistory.size(); ++i) {
            maxValue = qMax(maxValue, m_spectralFluxHistory[i] / 8000.0F);
        }

        // check if maxValue is below noise threshold:
        if (maxValue < AGC_NOISE_THRESHOLD || maxValue <= 0) {
            // do not change current gain
            return;
        }

        // calculate required gain:
        const float requiredGain = (1 - AGC_HEADROOM) / maxValue;

        // adjust gain in small steps:
        if (requiredGain < m_agcValue) {
            m_spectralFluxAgcValue = qMax(AGC_MIN_GAIN, qMax(requiredGain, m_spectralFluxAgcValue - AGC_DECREMENT_STEPSIZE));
        } else {
            m_spectralFluxAgcValue = qMin(AGC_MAX_GAIN, qMin(requiredGain, m_spectralFluxAgcValue + AGC_INCREMENT_STEPSIZE));
        }
    }
}

void SpeechInputAnalyzer::updateAGCWithoutFlux() {
    // spectrum AGC:
    {
        // get max of last values:
        float maxValue = 0.0;
        for (int i=0; i<m_lastMaxValues.size(); ++i) {
            maxValue = qMax(maxValue, m_lastMaxValues[i]);
        }

        // check if maxValue is below noise threshold:
        if (maxValue < AGC_NOISE_THRESHOLD || maxValue <= 0) {
            // do not change current gain
            return;
        }

        // calculate required gain:
        const float requiredGain = (1 - AGC_HEADROOM) / maxValue;

        // adjust gain in small steps:
        if (requiredGain < m_agcValue) {
            m_agcValue = qMax(AGC_MIN_GAIN, qMax(requiredGain, m_agcValue - AGC_DECREMENT_STEPSIZE));
        } else {
            m_agcValue = qMin(AGC_MAX_GAIN, qMin(requiredGain, m_agcValue + AGC_INCREMENT_STEPSIZE));
        }
    }
}
