#include "AudioInputAnalyzer.h"

#include "core/MainController.h"

#include <QDebug>

// ------------------------------- Utility Functions for BPM Detection -------------------------------

inline float bpmToMs(const float bpm) {
    return 60000.0f / bpm;
}

inline float msToBPM(const float ms) {
    return 60000.0f / ms;
}

inline int frequencyToIndex(const int frequency) {
    return SHORT_NUM_SAMPLES * frequency / AUDIO_SAMPLING_RATE;
}

inline int framesToMs(const int frames) {
    return frames * SAMPLES_BETWEEN_SPECTRUM_UPDATES * 1000 / AUDIO_SAMPLING_RATE;
}

inline int msToFrames(const int ms) {
    return ms * AUDIO_SAMPLING_RATE / SAMPLES_BETWEEN_SPECTRUM_UPDATES / 1000;
}

constexpr int GLOBAL_MIN_BPM = 50;
constexpr int GLOBAL_MAX_BPM = 300;


// ---------------------------------- AudioInputAnalyzer --------------------------------

AudioInputAnalyzer::AudioInputAnalyzer(QAudioDeviceInfo inputInfo, int channelIndex, QString name, MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_referenceList()
    , m_audioInput(nullptr)
    , m_audioRecordDevice(nullptr)
    , m_deviceInfo(inputInfo)
    , m_deviceName(name)
    , m_channelIndex(channelIndex)
    , m_analyzedTillIndex(0)
    , m_circBuffer(CIRC_BUFFER_LENGTH)
    , m_longBuffer(LONG_NUM_SAMPLES)
    , m_longWindow(LONG_NUM_SAMPLES)
    , m_longFftOutput(LONG_NUM_SAMPLES)
    , m_longSpectrum(LONG_NUM_SAMPLES / 2)
    , m_shortBuffer(SHORT_NUM_SAMPLES)
    , m_shortWindow(SHORT_NUM_SAMPLES)
    , m_lastShortFftOutput(SHORT_NUM_SAMPLES)
    , m_shortFftOutput(SHORT_NUM_SAMPLES)
    , m_shortSpectrum(SHORT_NUM_SAMPLES / 2)
    , m_spectrumHistory(SPECTRUM_HISTORY_LENGTH)
    , m_newSpectrumCount(0)
    , m_simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    , m_maxLevel(0.0)
    , m_agcValue(1.0)
    , m_spectralFluxAgcValue(1.0)
    , m_compression(1.0)
    , m_lastMaxValues(AGC_AVERAGING_LENGTH)
    , m_spectralFluxHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralColorHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralFluxNormalized(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_currentSpectralFlux(0.0)
    , m_onsetBuffer(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_detectedOnsets()
    , m_bpm(120)
    , m_callsSinceLastBpmUpdate(0)
    , m_lastBpmDetection()
    , m_detectBpm(false)
    , m_agents()
    , m_lastIntervals(INTERVALS_TO_STORE)
    , m_isRecordingSpeech(false)
{
    m_circBuffer.fill(0.0, m_circBuffer.capacity());
    m_spectralFluxHistory.fill(0.0, m_spectralFluxHistory.capacity());
    m_spectralColorHistory.fill(0.0, m_spectralColorHistory.capacity());
    calculateWindows();
    initAudio(inputInfo);
}

AudioInputAnalyzer::~AudioInputAnalyzer() {
    if (!m_audioInput.isNull()) {
        m_audioInput->stop();
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }
}

AudioInputAnalyzer::AudioInputAnalyzer(QString name, MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_referenceList()
    , m_audioInput(nullptr)
    , m_audioRecordDevice(nullptr)
    , m_deviceName(name)
    , m_channelIndex(0)
    , m_analyzedTillIndex(0)
    , m_circBuffer(0)
    , m_spectrumHistory(SPECTRUM_HISTORY_LENGTH)
    , m_newSpectrumCount(0)
    , m_simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    , m_maxLevel(0.0)
    , m_agcValue(1.0)
    , m_spectralFluxAgcValue(1.0)
    , m_compression(1.0)
    , m_lastMaxValues(AGC_AVERAGING_LENGTH)
    , m_spectralFluxHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralColorHistory(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_spectralFluxNormalized(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_currentSpectralFlux(0.0)
    , m_onsetBuffer(SPECTRAL_FLUX_HISTORY_LENGTH)
    , m_detectedOnsets()
    , m_bpm(0)
    , m_callsSinceLastBpmUpdate(0)
    , m_lastBpmDetection(HighResTime::now())
    , m_detectBpm(false)
    , m_agents()
    , m_lastIntervals(INTERVALS_TO_STORE)
    , m_isRecordingSpeech(false)
{

}

void AudioInputAnalyzer::addReference(void* ref) {
    if (m_referenceList.isEmpty()) {
        // this is the first registered object
        // start audio capture to m_audioRecordDevice:
        m_circBuffer.fill(0.0, m_circBuffer.capacity());
        m_spectralFluxHistory.fill(0.0, m_spectralFluxHistory.capacity());
        m_spectralColorHistory.fill(0.0, m_spectralColorHistory.capacity());
        createAudioInputForMusic();
        if (!m_audioInput) return;
        if (m_audioInput->volume() < 1.0) m_audioInput->setVolume(1.0);
        m_audioRecordDevice = m_audioInput->start();
        if (!m_audioRecordDevice) {
            qCritical() << "Could not start audio capture.";
            qCritical() << m_audioInput->error();
            return;
        } else {
            connect(m_audioRecordDevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
            connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(updateSpectrum()));
        }
    }
    m_referenceList.insert(ref);
}

void AudioInputAnalyzer::removeReference(void* ref) {
    m_referenceList.remove(ref);
    if (m_referenceList.isEmpty()) {
        // this was the last registered object
        if (m_audioRecordDevice) {
            disconnect(m_audioRecordDevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
            disconnect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(updateSpectrum()));
        }
        if (m_audioInput) {
            m_audioInput->stop();
            m_audioInput->deleteLater();
            m_audioInput = nullptr;
            m_audioRecordDevice = nullptr;  // is deleted by QAudioInput
        }
    }
}

void AudioInputAnalyzer::addReferenceForBpm(void* ref) {
    if (m_bpmReferenceList.isEmpty()) {
        // this is the first registered object for BPM
        m_detectBpm = true;
    }
    addReference(ref);
    m_bpmReferenceList.insert(ref);
    m_detectBpm = !m_bpmReferenceList.isEmpty();
}

void AudioInputAnalyzer::removeReferenceForBpm(void* ref) {
    removeReference(ref);
    m_bpmReferenceList.remove(ref);
    if (m_bpmReferenceList.isEmpty()) {
        // this was the last registered object for BPM
        m_detectBpm = false;
        m_spectralColorHistory.fill(QColor(0, 0, 0));
    }
    m_detectBpm = !m_bpmReferenceList.isEmpty();
}

void AudioInputAnalyzer::startSpeechRecording() {
    m_speechBuffer.clear();
    m_speechBufferPart.clear();

    if (!m_referenceList.isEmpty()) {
        qWarning() << "Can't record speech using this input because it is used for auido analysis.";
        return;
    }
    if (m_isRecordingSpeech) {
        qWarning() << "Already recording.";
        return;
    }

    createAudioInputForSpeech();
    if (!m_audioInput) return;
    if (m_audioInput->volume() < 1.0) m_audioInput->setVolume(1.0);
    m_audioRecordDevice = m_audioInput->start();
    if (!m_audioRecordDevice) {
        qCritical() << "Could not start audio capture.";
        qCritical() << m_audioInput->error();
        return;
    } else {
        m_isRecordingSpeech = true;
        connect(m_audioRecordDevice, SIGNAL(readyRead()), this, SLOT(speechDataReady()));
    }
}

const QByteArray& AudioInputAnalyzer::stopSpeechRecording() {
    if (!m_isRecordingSpeech) return m_speechBuffer;
    if (m_audioRecordDevice) {
        disconnect(m_audioRecordDevice, SIGNAL(readyRead()), this, SLOT(speechDataReady()));
    }
    if (m_audioInput) {
        m_audioInput->stop();
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
        m_audioRecordDevice = nullptr;  // is deleted by QAudioInput
    }
    m_isRecordingSpeech = false;
    return m_speechBuffer;
}

float AudioInputAnalyzer::getLevelAtBand(double band) const {
    return m_simplifiedSpectrum[band * (m_simplifiedSpectrum.size() - 1)];
}

void AudioInputAnalyzer::calculateWindows() {
    // Hann Window function
    for (std::size_t i=0; i<LONG_NUM_SAMPLES; ++i) {
        m_longWindow[i] = 0.5f * (1 - qCos((2 * M_PI * i) / (LONG_NUM_SAMPLES - 1)));
    }
    for (std::size_t i=0; i<SHORT_NUM_SAMPLES; ++i) {
        m_shortWindow[i] = 0.5f * (1 - qCos((2 * M_PI * i) / (SHORT_NUM_SAMPLES - 1)));
    }
}

void AudioInputAnalyzer::initAudio(QAudioDeviceInfo info) {
    // fill circular buffer with zeros:
    for (int i=0; i < m_circBuffer.capacity(); ++i) {
        m_circBuffer.push_back(0.0);
    }

}

void AudioInputAnalyzer::createAudioInputForMusic() {
    if (m_audioInput) {
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }

    // Set up the desired audio input format:
    QAudioFormat format;
    format.setSampleRate(AUDIO_SAMPLING_RATE);
    format.setChannelCount(m_deviceInfo.preferredFormat().channelCount());
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // check if the format is support:
    if (!m_deviceInfo.isFormatSupported(format)) {
        // it is not -> use the nearest supported one:
        qWarning() << "Default audio format not supported, trying to use the nearest.";
        format = m_deviceInfo.nearestFormat(format);
    }
    m_audioFormat = format;

    // create a QAudioInput object:
    m_audioInput = new QAudioInput(m_deviceInfo, format);
    if (!m_audioInput) {
        qCritical() << "Could not open audio device.";
        return;
    }
}

void AudioInputAnalyzer::createAudioInputForSpeech() {
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
    }
    m_audioFormat = format;

    // create a QAudioInput object:
    m_audioInput = new QAudioInput(m_deviceInfo, format);
    if (!m_audioInput) {
        qCritical() << "Could not open audio device.";
        return;
    }
}

void AudioInputAnalyzer::setBpm(float value) {
    m_bpm = value;
    m_lastBpmDetection = HighResTime::now();
    emit bpmChanged();
}

float AudioInputAnalyzer::bpmInRange(float bpm, const int minBPM) {
    if (minBPM > 0) {
        while (bpm < minBPM && bpm != 0) {
            bpm *= 2;
        }
        while (bpm >= minBPM*2) {
            bpm /= 2;
        }
    }

    while (bpm < GLOBAL_MIN_BPM && bpm != 0) {
        bpm *= 2;
    }
    while (bpm >= GLOBAL_MAX_BPM) {
        bpm /= 2;
    }

    return bpm;
}

void AudioInputAnalyzer::updateSpectrum() {
    if (m_spectrumHistory.isEmpty()) return;
    m_newSpectrumCount = qMax(0, m_newSpectrumCount - 1);
    int currentIndex = qMax(0, m_spectrumHistory.size() - 1 - m_newSpectrumCount);
    m_simplifiedSpectrum = m_spectrumHistory[currentIndex];
    int fluxIndex = qMax(0, m_spectralFluxHistory.size() - 1 - m_newSpectrumCount);
    m_currentSpectralFlux = m_spectralFluxHistory[fluxIndex];
    emit spectrumChanged();
}

void AudioInputAnalyzer::audioDataReady() {
    if (!m_audioRecordDevice) return;
    // read data from input as QByteArray:
    QByteArray data = m_audioRecordDevice->readAll();
    const int bytesPerSample = m_audioFormat.bytesPerFrame();
    const int numSamples = data.size() / bytesPerSample;
    const char *ptr = data.constData();
    ptr += m_audioFormat.sampleSize() * m_channelIndex / 8;

    if (m_audioFormat.sampleSize() <= 16) {  // 16 bit

        // iterate over raw input data:
        for (int i=0; i<numSamples; ++i) {
            // interpret byte in QByteArray as int16:
            const int16_t pcmSample = *reinterpret_cast<const int16_t*>(ptr);
            // convert to float and scale down to range [-1.0, 1.0]:
            const float scaled = float(pcmSample) / 32768;
            // push to circular buffer:
            m_circBuffer.push_back(scaled);
            ptr += bytesPerSample;
        }

    } else { // 24 bit

        // iterate over raw input data:
        for (int i=0; i<numSamples; ++i) {
            // interpret byte in QByteArray as int32:
            const int32_t pcmSample = *reinterpret_cast<const int32_t*>(ptr);
            // convert to float and scale down to range [-1.0, 1.0]:
            const float scaled = float(pcmSample) / 8388608;
            // push to circular buffer:
            m_circBuffer.push_back(scaled);
            ptr += bytesPerSample;
        }
    }

    m_analyzedTillIndex = qMax(0, m_analyzedTillIndex - numSamples);

    analyzeNewSamples();
}

void AudioInputAnalyzer::speechDataReady() {
    if (!m_audioRecordDevice) return;
    // read data from input as QByteArray:
    QByteArray data = m_audioRecordDevice->readAll();
    m_speechBuffer.append(data);
    m_speechBufferPart.append(data);
    if (m_speechBufferPart.size() > 10000) {
        emit speechPartReady(m_speechBufferPart);
        m_speechBufferPart.clear();
    }
}

void AudioInputAnalyzer::analyzeNewSamples() {
    m_newSpectrumCount = 0;
    while (CIRC_BUFFER_LENGTH - m_analyzedTillIndex >= SAMPLES_BETWEEN_SPECTRUM_UPDATES) {
        createSpectrumTillIndex(m_analyzedTillIndex + SAMPLES_BETWEEN_SPECTRUM_UPDATES);

        m_analyzedTillIndex += SAMPLES_BETWEEN_SPECTRUM_UPDATES;
    }

    // ----------------- BPM Detection Steps --------------

    if (!m_detectBpm) return;

    updateOnsets();
    emit spectralFluxHistoryChanged();

    // Use a counter to only perform the tempo detection calculations every n times, because they are expensive
    m_callsSinceLastBpmUpdate++;
    if (m_callsSinceLastBpmUpdate >= CALLS_TO_WAIT_FOR_BPM) {
        m_callsSinceLastBpmUpdate = 0;
        // Retrieve a set of clusters that group similar and related inter offset intervals
        updateAgents();

        // Find the highest scored cluster and declare it the bpm
        evaluateAgents();
    }
}

void AudioInputAnalyzer::createSpectrumTillIndex(std::size_t endIndex) {
    if (endIndex < LONG_NUM_SAMPLES) return;
    updateRawSpectrumsTillIndex(endIndex);
    createSimplifiedSpectrumFromRawSpectrums();
}

void AudioInputAnalyzer::updateRawSpectrumsTillIndex(std::size_t endIndex) {
    // -------- Long for low frequencies:

    // copy data from circular buffer to m_longBuffer and apply window:
    for (std::size_t i=0; i < LONG_NUM_SAMPLES; ++i) {
        m_longBuffer[i] = m_circBuffer[(endIndex - LONG_NUM_SAMPLES) + i] * m_longWindow[i];
    }
    // apply FFT to m_longBuffer and write result to m_longFftOutput:
    m_longFftreal.do_fft(m_longFftOutput.data(), m_longBuffer.data());

    // iterate over FFT result:
    for (std::size_t i=0; i < LONG_NUM_SAMPLES / 2; ++i) {
        // calculate magnitude for each FFT bin:
        const float real = m_longFftOutput[i];
        const float img = m_longFftOutput[LONG_NUM_SAMPLES / 2 + i];
        //const float magnitudeInDb = 4.342f * std::log10(real*real + img*img);  // simplified from 20*log10(sqrt(r*r+i*i)) -> dB of Magnitude
        //const float magnitude = std::pow(real*real + img*img, 0.2) / 14;  // 14 is most of the time the max value
        const float magnitude = std::pow(real*real + img*img, 0.3f) / 60;  // 40 is most of the time the max value
        // write normalized magnitude to m_longSpectrum:
        m_longSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));
    }

    // -------- Short for high frequencies:

    // copy data from circular buffer to m_shortBuffer and apply window:
    for (std::size_t i=0; i < SHORT_NUM_SAMPLES; ++i) {
        m_shortBuffer[i] = m_circBuffer[(endIndex - SHORT_NUM_SAMPLES) + i] * m_shortWindow[i];
    }
    // apply FFT to m_shortBuffer and write result to m_shortFftOutput:
    m_shortFftreal.do_fft(m_shortFftOutput.data(), m_shortBuffer.data());

    // Calculates the spectral flux for the samples from the given index
    // Spectral flux is the sum of only the *increases* in frequency.
    // See "Evaluation of the Audio Beat Tracking System BeatRoot" by Simon Dixon
    // (in Journal of New Music Research, 36, 2007/8) for further detail
    float flux = 0.0;

    float max = 0.0;

    // iterate over FFT result:
    for (std::size_t i=0; i < SHORT_NUM_SAMPLES / 2; ++i) {
        // calculate magnitude for each FFT bin:
        const float real = m_shortFftOutput[i];
        const float img = m_shortFftOutput[SHORT_NUM_SAMPLES / 2 + i];
        //const float magnitudeInDb = 4.342f * std::log10(real*real + img*img);  // simplified from 20*log10(sqrt(r*r+i*i)) -> dB of Magnitude
        //const float magnitude = std::pow(real*real + img*img, 0.2) / 9;  // 9 is most of the time the max value
        const float magnitude = std::pow(real*real + img*img, 0.3f) / 32;  // 32 is most of the time the max value
        // check if this is the current maximum value:
        max = std::max(max, magnitude);
        // write normalized magnitude to m_shortSpectrum:
        m_shortSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));

        if (m_shortFftOutput[i] > m_lastShortFftOutput[i]) {
            flux += m_shortFftOutput[i] - m_lastShortFftOutput[i];
        }
    }
    m_maxLevel = limit(0.0f, max, 1.0f);
    m_spectralFluxHistory.push_back(flux);
    m_lastShortFftOutput = m_shortFftOutput;

    // ----- Automatic Gain Control:
    m_lastMaxValues.push_back(max);
    updateAGC();
}

void AudioInputAnalyzer::createSimplifiedSpectrumFromRawSpectrums() {
    const int beginFreq = 10;
    const double factor = 1.061989883394314;  // pow(22050 / 10., 1/128.)
    //const double factor = 1.0305289337977435;  // pow(22050 / 10., 1/256.)
    //const float maxPossibleEnergy = (MAX_FFT_VALUE*qMax(std::size_t(1), valuesTillNext));
    //const float longMaxPossibleEnergy = LONG_MAX_FFT_VALUE;
    //const float shortMaxPossibleEnergy = SHORT_MAX_FFT_VALUE;

    std::vector<double> simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH);
    double freq = beginFreq;

    for (std::size_t i=0; i<SIMPLIFIED_SPECTRUM_LENGTH; ++i) {
        const double nextFreq = beginFreq * std::pow(factor, i+1);
        if (freq < 200) {
            const std::size_t startIndex = freq / 22050 * (LONG_NUM_SAMPLES / 2);
            const std::size_t endIndex = nextFreq / 22050 * (LONG_NUM_SAMPLES / 2);
            const std::size_t valuesTillNext = endIndex - startIndex;
            if (endIndex >= LONG_NUM_SAMPLES) break;

            // find max value in this range:
            float max = m_longSpectrum[startIndex];
            for (std::size_t j=1; j < valuesTillNext; ++j) {
                max = std::max(max, m_longSpectrum[startIndex+j]);
            }
            simplifiedSpectrum[i] = max;

//            // Sum up the energies of all FFT elements between the two frequencies:
//            float sumEnergy = m_longSpectrum[startIndex];
//            for (int j=1; j < valuesTillNext; ++j) {
//                sumEnergy += m_longSpectrum[startIndex+j];
//            }

//            // Apply reasonable scale:
//            sumEnergy /= longMaxPossibleEnergy;

//            // Scale the value with factor and exponent:
//            simplifiedSpectrum[i] = qPow(qMax(0.0f, qMin(sumEnergy, 1.0f)), (1 / m_compression));

        } else {
            const std::size_t startIndex = freq / 22050 * (SHORT_NUM_SAMPLES / 2);
            const std::size_t endIndex = nextFreq / 22050 * (SHORT_NUM_SAMPLES / 2);
            const std::size_t valuesTillNext = endIndex - startIndex;
            if (endIndex >= SHORT_NUM_SAMPLES) continue;

            // find max value in this range:
            float max = m_shortSpectrum[startIndex];
            for (std::size_t j=1; j < valuesTillNext; ++j) {
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
        }
        freq = nextFreq;
    }
    //simplifiedSpectrum[50] = 1;  // point between long and short spectrum (200 Hz)

    // ----------------------- Calculate Spectral Color -----------------------

    if (m_detectBpm) {
        double redMax = 0;
        double greenMax = 0;
        double blueMax = 0;
        for (std::size_t i=0; i < (SIMPLIFIED_SPECTRUM_LENGTH * 0.3); ++i) {
            redMax = qMax(redMax, simplifiedSpectrum[i]);
        }
        for (std::size_t i=std::size_t(SIMPLIFIED_SPECTRUM_LENGTH * 0.3); i < (SIMPLIFIED_SPECTRUM_LENGTH * 0.6); ++i) {
            greenMax = qMax(greenMax, simplifiedSpectrum[i]);
        }
        for (std::size_t i=std::size_t(SIMPLIFIED_SPECTRUM_LENGTH * 0.6); i < SIMPLIFIED_SPECTRUM_LENGTH; ++i) {
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

    // push spectrum to spectrum history with move semantic:
    m_spectrumHistory.push_back(move(simplifiedSpectrum));
    // Attention: because of move semantic simplifiedSpectrum is now unusable!
    ++m_newSpectrumCount;
}


// finds onsets in the five bands of audio material and connect them with an or
// Algorithm is again from "Evaluation of the Audio Beat Tracking System BeatRoot"
// by Simon Dixon (in Journal of New Music Research, 36, 2007/8), where the same
// approach has been applied without the differentiation into five bands
void AudioInputAnalyzer::updateOnsets() {
    // clear the onset history
    for (int i=0; i < SPECTRAL_FLUX_HISTORY_LENGTH; ++i) {
        m_onsetBuffer[i] = false;
    }
    m_detectedOnsets.clear();
    // normalize the spectral flux to an average of 0 and a standard deviation of 1,
    // by determining the current average and standard deviation and then subtracting
    // the average from each value and dividing it by the standard Deviation
    //
    // this is done not per frequency band, but over all bands to prevent a lot of false
    // positives from the noise floor of one silent band being amplified to much, without
    // using any absoulute thresholds
    float average = 0.0;
    for (int i=0; i < SPECTRAL_FLUX_HISTORY_LENGTH; ++i) {
        average += m_spectralFluxHistory[i];
    }
    average = average / SPECTRAL_FLUX_HISTORY_LENGTH;

    float variance = 0.0;
    for (int i = 0; i < SPECTRAL_FLUX_HISTORY_LENGTH; ++i) {
        variance += m_spectralFluxHistory[i]*m_spectralFluxHistory[i];
    }
    float stdDev = qSqrt(variance);

    // Cap the standard Deviation to prevent onset detection in ADC Noise Floor, which has a stdDev of under 20 (music is usually over 100, to about 30000)
    stdDev = qMax(stdDev, 20.0f);

    for (int i = 0; i < SPECTRAL_FLUX_HISTORY_LENGTH; ++i) {
        m_spectralFluxNormalized[i] = (m_spectralFluxHistory[i] - average) / stdDev;
    }

    const char w = 5; // window for local maximum detection
    const char m = 3; // multiplier to increase range before onset
    const float pastThresholdWeight = 0.84f;
    const float averageThresholdDelta = 0.008f;

    // detect onsets by iterating over spectral flux and checking for the three criteria
    // that a sample must fullfill to be considered an onset
    // 1. Past Threshold: have a greater value than the (g in the paper)
    // 2. Local Maximum: have value greater than the neighboring samples within a window of +- w samples
    // 3. Average Threshold: have a greater value than the average of the surrounding samples (-m*w to +w) with an added Delta
    // The order is changed from the paper, to ensure calculation of the recursive function, while keeping
    // the code easy to read

    float pastThreshold = m_spectralFluxNormalized[m*w-1];

    // Iterate over all samples except the edges where there are not enough surrounding samples
    for (int n=m*w; n < SPECTRAL_FLUX_HISTORY_LENGTH - w; ++n) {

        // ------------------------------- 1. Past Threshold -----------------------------
        // Calculate the past threshold recursively, as the maximum between a weighted average between
        // the last threshold and the last sample, and the last sample itself
        float pastThresholdNew = qMax(m_spectralFluxNormalized[n-1], pastThresholdWeight*pastThreshold + (1-pastThresholdWeight)*m_spectralFluxNormalized[n-1]);
        pastThreshold = pastThresholdNew;

        // Continue if the sample does not meet the past threshold
        if (m_spectralFluxNormalized[n] < pastThreshold) {
            continue;
        }

        // -------------------------------- 2. Local Maximum -----------------------------
        // Compare to the releavant surronding samples
        bool localMaximum = true;
        for (int k = n-w; k <= n+w; ++k) {
            if (m_spectralFluxNormalized[n] < m_spectralFluxNormalized[k]) {
                localMaximum = false;
                break;
            }
        }

        // Continue if the sample does not meet the local neighbour criterium
        if (!localMaximum) {
            continue;
        }

        // ---------------------------- 3. Average Threshold -----------------------------
        // Sum the surounding samples, then divide by their number and add the delta
        float averageThreshold = 0.0;
        for (int k = n-w*m; k < n+w; ++k) {
            averageThreshold += m_spectralFluxNormalized[k];
        }
        averageThreshold /= (m*w + w +1);
        averageThreshold += averageThresholdDelta;

        // Continue if the sample does not meet the average threshold
        if (m_spectralFluxNormalized[n] < averageThreshold) {
            continue;
        }

        // Set the sample to be an onset if it has met all the criteria
        m_onsetBuffer[n] = true;

        m_detectedOnsets.append(double(n) / SPECTRAL_FLUX_HISTORY_LENGTH);
    }
}



// ---------------------------- BPM Detection with Beat Agents ------------------------------


const static int CLUSTER_WIDTH = 30; // ms
const static int CLUSTER_WIDTH_IN_FRAMES = msToFrames(CLUSTER_WIDTH); // frames
const static int MAX_INTERVAL = 2000; // ms

// A class that models a Beat Agent
class BeatAgent {

public:
    BeatAgent(int interval, float score) :
        m_averageInterval(interval)
      , m_size(1)
      , m_nativeScore(score)
    {}

    float getSize() { return m_size; }
    float getScore() { return m_nativeScore; }
    float getAverageInterval() { return m_averageInterval; }

    void addInterval(float interval, float score) {
        m_averageInterval = (m_size * m_averageInterval + interval) / (m_size + 1);
        m_nativeScore += score;
        ++m_size;
    }

    bool operator==(const BeatAgent& other) {
        return m_averageInterval == other.m_averageInterval
                && m_size == other.m_size
                && m_nativeScore == other.m_nativeScore;
    }

protected:
    float   m_averageInterval;
    int     m_size;
    float   m_nativeScore;
};


class IntervalCluster {

public:
    IntervalCluster(int interval, float score) :
        m_averageInterval(interval)
      , m_size(1)
      , m_nativeScore(score)
    {}

    float getScore() { return m_nativeScore; }
    float getAverageInterval() { return m_averageInterval; }

    void addInterval(float interval, float score) {
        m_averageInterval = (m_size * m_averageInterval + interval) / (m_size + 1);
        m_nativeScore += score;
        ++m_size;
    }

    bool operator==(const IntervalCluster& other) {
        return m_averageInterval == other.m_averageInterval
                && m_size == other.m_size
                && m_nativeScore == other.m_nativeScore;
    }

protected:
    float   m_averageInterval;
    int     m_size;
    float   m_nativeScore;
};


// Identifies BeatAgents (strings of Onsets with roughly consistent Intervals
// from the detected onsets. Loosely Based on "Automatic Extraction of Tempo
// and beat from Expressive Performances" by Simon Dixon (2001)
void AudioInputAnalyzer::updateAgents() {
    // Deleta all existing agents
    m_agents.clear();

    // Iterate of all pairs of indices i and j, where there is an onset at
    // both indices and i < j. Then try to find as many onsets as possible
    // with the same equal interval:
    //
    // In this illustration, the i/j/x are the onsets in the signal, and the
    // stars mark all the consecutive onsets with the same interval that
    // could be found in the signal.
    //
    // *    *    *    *    *    *    *
    // i----j-x--x----x-x--x----x--x-------x--x--x-x----x
    for (int i = 0; i < SPECTRAL_FLUX_HISTORY_LENGTH; ++i) {
        if (m_onsetBuffer[i]) {
            for (int j = i+1; j < SPECTRAL_FLUX_HISTORY_LENGTH; ++j) {
                if (m_onsetBuffer[j]) {

                    // Detect the interval and score, and continue on if the interval is too short or too long
                    int interval = framesToMs(j-i);
                    if (!(CLUSTER_WIDTH < interval && interval < MAX_INTERVAL)) {
                        continue;
                    }
                    float score = qMin(m_spectralFluxNormalized[i], m_spectralFluxNormalized[j]);

                    // Initialize a new Beat Agent with tha interval and score
                    BeatAgent agent(interval, score);

                    // Store the index of the last onset (start with j)
                    int lastOnsetIndex = j;
                    // Get the margin of tolerance
                    float minInterval = agent.getAverageInterval() - CLUSTER_WIDTH;
                    float maxInterval = agent.getAverageInterval() + CLUSTER_WIDTH;
                    // Allow to skip one beat
                    bool skipedBeat = false;

                    // Iterate over all future indices
                    for (int k = j+msToFrames(minInterval); k < SPECTRAL_FLUX_HISTORY_LENGTH; ++k) {
                        // Calculate the interval from the last onset
                        float interval = framesToMs(k-lastOnsetIndex);

                        // If the interval became to long, simulate a last beat to allow one missing one
                        // or break if this has already been the done
                        if (interval > maxInterval) {
                            if (skipedBeat) {
                                break;
                            } else {
                                lastOnsetIndex += msToFrames(agent.getAverageInterval());
                                // Skip ahead the minimal distance two onsets may be apart
                                skipedBeat = true;
                                k+= qMax(msToFrames(minInterval - CLUSTER_WIDTH) - 1, 0);
                                continue;
                            }
                        }
                        // If an onset was found, update the agent and set it as the last onset
                        if (m_onsetBuffer[k]) {
                            // The score is the minimum of the two onsets spectral fluxes
                            float score = qMin(m_spectralFluxNormalized[lastOnsetIndex], m_spectralFluxNormalized[k]);
                            agent.addInterval(interval, score);
                            lastOnsetIndex = k;

                            // Recalculate the margin of tolerance from the new interval
                            minInterval = agent.getAverageInterval() - CLUSTER_WIDTH;
                            maxInterval = agent.getAverageInterval() + CLUSTER_WIDTH;

                            // Skip ahead the minimal distance two onsets may be apart
                            k += qMax(msToFrames(minInterval) - 1, 0);
                        }
                    }


                    // Discard the agent if he doesn't have at least 4 beats
                    if (agent.getSize() < 3) {
                        continue;
                    }

                    bool discardAgent = false;
                    // See if there is another agent that is within cluster width. If it is scored higher, delete it. Else, mark the new one to be deleted
                    for (auto k = m_agents.begin(); k != m_agents.end(); ++k) {
                        if (qAbs(k->getAverageInterval() - agent.getAverageInterval()) < CLUSTER_WIDTH) {
                            if (k->getScore() > agent.getScore()) {
                                discardAgent = true;
                                break;
                            } else {
                                // Save a reference to j to remove it from the list
                                BeatAgent& agentToDie = *k;
                                // Decrease the iterator in order not to point to an item that is will be gone
                                --k;
                                // Remove j from the list
                                m_agents.removeOne(agentToDie);

                            }
                            break;
                        }
                    }

                    if (!discardAgent) {
                        m_agents.append(agent);
                    }
                }
            }
        }
    }
}


// Checks a given interval for sufficent support in the agents,
// to evaluate if after a drastic tempo change the old tempo is still plausible
// This is the case if there is an agent within CLUSTER_WIDTH of the interval
// that has at least 75% the score of the other interval
BeatAgent* AudioInputAnalyzer::plausibleAgentForInterval(float interval, float maxScore) {
    for (BeatAgent& agent : m_agents) {
        if (qAbs(agent.getAverageInterval() - interval) < CLUSTER_WIDTH) {
            if (agent.getScore() >= 0.75f * maxScore) {
                return &agent;
            }
        }
    }

    return 0;
}

static const float fractionsToCheck[] = {4.0f/3.0f,
                                         2.0f/3.0f,
                                         2.0f,
                                         0.5f,
                                         3.0f};

// evaluates the agents by using the highest scored agents interval to calculate the bpm
void AudioInputAnalyzer::evaluateAgents() {
    BeatAgent* maxAgent = 0;
    for (BeatAgent& cluster : m_agents) {
        if (!maxAgent || cluster.getScore() > maxAgent->getScore()) {
            maxAgent = &cluster;
        }
    }

    // Only change the value if a agent has been identified as the winner
    if (maxAgent) {
        // Itendify the interval
        float newInterval = maxAgent->getAverageInterval();

        // If the new tempo is relevantley different from the old one. Check common fractions by
        // which the tempo is likely to deviate from what is probably the actual tempo (represented
        // by the last recognized tempo as our best guess)
        if ( m_lastIntervals.size() && qAbs(newInterval - m_lastIntervals.last()) > CLUSTER_WIDTH)
        {
            for ( const float& fraction : fractionsToCheck )
            {
                // Check if the difference is small enough
                if ( (qAbs(fraction*newInterval - m_lastIntervals.last()) < 2*CLUSTER_WIDTH) )
                {
                    // Check if there is another agent that suggest strong enough evidence that the multiplied tempo could be the actual tempo
                    BeatAgent* plausibleAgent = plausibleAgentForInterval(m_lastIntervals.last(), maxAgent->getScore() / fraction);
                    if (plausibleAgent) {
                        newInterval = plausibleAgent->getAverageInterval();
                        break;
                    }
                }
            }
        }
        m_lastIntervals.append(newInterval);

        // Perform clustering on the last bpms to smooth out any spikes, prefering new values
        QLinkedList<IntervalCluster> finalIntervalClusters;
        for (float& interval : m_lastIntervals) {
            // Identify the cluster that most closely matches the interval (up to CLUSTER_WIDTH deviation is allowd)
            IntervalCluster* closestCluster = 0;
            int closestDistance = INT_MAX;
            for (IntervalCluster& cluster : finalIntervalClusters) {
                int distance = qAbs(cluster.getAverageInterval() - interval);
                if (distance < CLUSTER_WIDTH && distance < closestDistance) {
                    closestDistance = distance;
                    closestCluster = &cluster;
                }
            }

            //If a matching cluster was found, add the interval to it. If not, create a new cluster
            if (closestCluster) {
                closestCluster->addInterval(interval, 1.0);
            } else {
                finalIntervalClusters.append(IntervalCluster(interval, 1.0));
            }
        }

        // Identify the winning cluster of tempos
        IntervalCluster* maxFinalCluster = 0;
        for (IntervalCluster& cluster : finalIntervalClusters) {
            if (!maxFinalCluster || cluster.getScore() > maxFinalCluster->getScore()) {
                maxFinalCluster = &cluster;
            }
        }

        // Only call the cluster winning if it contains at least half the intervals. else keep the old tempo
        if (maxFinalCluster && maxFinalCluster->getScore()*2 > INTERVALS_TO_STORE) {
            float newBPM = msToBPM(maxFinalCluster->getAverageInterval());
            setBpm(newBPM);
            return;
        }
    }
    // if this point is reached, no BPM value could be detected
    emit bpmChanged();
}

void AudioInputAnalyzer::updateAGC() {
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
