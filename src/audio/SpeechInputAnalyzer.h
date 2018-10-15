#ifndef SPEECHINPUTANALYZER_H
#define SPEECHINPUTANALYZER_H

#include "utils.h"
#include "ffft/FFTRealFixLen.h"

#include "core/QCircularBuffer.h"
#include <QObject>
#include <QSet>
#include <QPointer>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QTimer>

#include <vector>

// power of two of SHORT_NUM_SAMPLES
static const int SPEECH_FFT_SAMPLES_EXPONENT = 9;  // 9 -> 512 samples for FFT

// ---------------------------------------------------

class MainController;  // forward declaration


// ---------------------------------- SpeechInputAnalyzer --------------------------------

/**
 * @brief The SpeechInputAnalyzer class is responsible for analyzing a single
 * audio input device using the Fast Fourier Transform (FFT) to generate a frequency spectrum.
 *
 * It uses a short sample bufffer for calculating the FFT for mid and high frequencies.
 *
 * It is optimized for speech input and hotword detection.
 */
class SpeechInputAnalyzer : public QObject {

    Q_OBJECT

    Q_PROPERTY(double currentLevel READ currentLevel NOTIFY currentLevelChanged)
    Q_PROPERTY(QVector<double> valueHistory READ getValueHistory NOTIFY currentLevelChanged)

public:

    /**
     * @brief SpeechInputAnalyzer creates an object of this class
     * @param inputInfo containing the information about the device to analyze
     * @param channelIndex 0 for left channel, 1 for right channel
     * @param name deviceName + channel name of this input
     * @param controller a pointer to the MainController
     */
    explicit SpeechInputAnalyzer(QAudioDeviceInfo inputInfo, int channelIndex, QString name, MainController* controller);
    ~SpeechInputAnalyzer();

    /**
     * @brief SpeechInputAnalyzer Used to create an empty / dummy analyzer.
     * @param controller a pointer to the MainController
     * @param name deviceName + channel name of this input
     */
    explicit SpeechInputAnalyzer(QString name, MainController* controller);

    /**
     * @brief addReference registers an object interested in audio capture
     *
     * This analyzer only starts continous analyzing when at least one object is registered
     * (reference counting).
     *
     * @param ref a pointer to the interested object
     */
    void addReference(void* ref);

    /**
     * @brief removeReference unregisters an interested object for audio capture
     * @param ref a pointer to an object previously registered with addReference()
     */
    void removeReference(void* ref);

signals:
    void isCapturingChanged();
    void currentLevelChanged();

    void isRecordingChanged();
    void speechPartReady(const QByteArray& rawAudio);  //!< is emitted when a chunk of speech data is ready

    void spectralFluxHistoryChanged();  //!< is emitted when the spectral flux history changed

    void spectrumChanged();  //!< is emitted when the spectrum changed


public slots:

    /**
     * @brief getDeviceName returns the name of the device that is analyzed
     * @return an input device name
     */
    QString getDeviceName() const { return m_deviceName; }

    // ------------------------ Capturing and Level -----------------------

    bool isCapturing() const;

    double currentLevel() const { return m_currentRMS; }

    // ------------------------ Speech Recording -----------------------

    /**
     * @brief startRecording starts speech recording with 16kHz sample rate
     *
     * periodically emits speechPartReady when ~1/10s speech was recorded to use for
     * streaming speech recognition
     */
    void startRecording();

    /**
     * @brief stopRecording stops speech recording and returns the full recording
     * @return full speech recording with 16kHz sample rate
     */
    const QByteArray& stopRecording();

    const QByteArray& getSpeechRecording() const { return m_speechRecording; }
    const QByteArray& getSpeechRecordingPart() const { return m_speechRecordingPart; }

    bool isRecording() const { return m_isRecording; }


    // ------------------------ Spectrum -----------------------

    /**
     * @brief getSimplifiedSpectrum returns a simplified scaled frequency spectrum
     * @return spectrum with energy values between 0 and 1
     */
    const std::vector<double>& getSimplifiedSpectrum() const;

    /**
     * @brief getSpectralFluxHistory returns the history of the spectral flux values,
     * typically the last 5s
     * @return an array of spectral flux values [0... ~8000]
     */
    const Qt3DCore::QCircularBuffer<float>& getSpectralFluxHistory() const { return m_spectralFluxHistory; }

    /**
     * @brief getSpectralColorHistory returns the history of the spectral color values,
     * typically the last 5s, matching the values of getSpectralFluxHistory()
     * @return an array of spectral color values
     */
    const Qt3DCore::QCircularBuffer<QColor>& getSpectralColorHistory() const { return m_spectralColorHistory; }

    /**
     * @brief getMaxLevel returns maximum level of all frequencies
     * @return a level between 0 and 1
     */
    float getMaxLevel() const { return m_maxLevel; }

    /**
     * @brief getCurrentSpectralFlux returns the current spectral flux value
     * @return normalized spectral flux value [0...1]
     */
    double getCurrentSpectralFlux() const { return limit(0, m_currentSpectralFlux / 8000.0, 1); }

    /**
     * @brief getAgcValue returns the gain that the Automatic Gain Control evaluate would be best
     * @return a gain value [0.5...~5]
     */
    double getAgcValue() const { return m_agcValue; }

    /**
     * @brief getSpectralFluxAgcValue returns the gain for spectral flux values
     * that the Automatic Gain Control evaluate would be best
     * @return a gain value [0.5...~5]
     */
    double getSpectralFluxAgcValue() const { return m_spectralFluxAgcValue; }

    QVector<double> getValueHistory() const;

private:
    /**
     * @brief calculateWindows intializes the arrays used for the windowing function before FFT
     */
    void calculateWindows();
    /**
     * @brief initAudio initializes the audio device capture
     * @param info information about the device to analyze
     */
    void initAudio(QAudioDeviceInfo info);

    /**
     * @brief createAudioInputForSpeech sets up the audio input for speech (16kHz sample rate)
     */
    void createAudioInputForSpeech();

    void destroyAudioInputForSpeech();

    void startAudioCapture();

    void stopAudioCapture();

private slots:

    void onAudioDataReady();

    void analyzeAudio();

    void calculateRawSpectrum();
    void calculateRawSpectrumWithoutFlux();

    void createSimplifiedSpectrumFromRawSpectrum();

    /**
     * @brief updateAGC calculates the required gain and changes the actual gain in small steps
     * based on the last maximum values of the FFT
     */
    void updateAGC();
    void updateAGCWithoutFlux();

protected:
    MainController* const m_controller;  //!< a pointer to the main controller

    QSet<void*> m_referenceList;  //!< list of registered objects

    QPointer<QAudioInput> m_audioInput;  //!< audio input device
    QPointer<QIODevice> m_audioCaptureIODevice;  //!< audio record device
    QAudioFormat m_audioFormat;  //!< format to record audio in
    QAudioDeviceInfo m_deviceInfo;  //!< audio device info
    QString m_deviceName;  //!< name of input device
    int m_channelIndex;  //!< 0 for left channel, 1 for right channel

    bool m_isRecording;  //!< true if this input is currently used to record speech
    QByteArray m_speechRecording;  //!< buffer storing the raw audio data while speech is recorded
    QByteArray m_speechRecordingPart;  //!< buffer storing the latest raw audio data while speech is recorded for streaming recognition

    // ------------- FFT -------------

    QTimer m_fftTimer;  //!< timer to trigger FFT generation

    Qt3DCore::QCircularBuffer<float> m_circBuffer;  //!< ring buffer to store incoming audio samples

    std::vector<float> m_shortBuffer;  //!< buffer for short FFT
    std::vector<float> m_shortWindow;  //!< window function array for short FFT
    ffft::FFTRealFixLen<SPEECH_FFT_SAMPLES_EXPONENT> m_shortFftreal;  //!< object for FFT calculations
    std::vector<float> m_lastShortFftOutput;  //!< previous output buffer for short FFT
    std::vector<float> m_shortFftOutput;  //!< output buffer for short FFT
    std::vector<float> m_shortSpectrum;  //!< resulting spectrum of short FFT

    Qt3DCore::QCircularBuffer<std::vector<double>> m_spectrumHistory;  //!< last spectrums
    std::vector<double> m_dummySpectrum;  //!< empty dummy spectrum


    // ------------- Spectral Flux -------------

    float m_currentRMS;  //!< current level of input device
    float m_maxLevel;  //!< maximum level of input device

    float m_agcValue;  //!< best amplifictation factor of spectrum
    float m_spectralFluxAgcValue;  //!< best amplifictation factor for spectral flux
    float m_compression;  //!< Compression factor (the higher it is the more spectrum gets compressed)
    Qt3DCore::QCircularBuffer<float> m_lastMaxValues;  //!< list of last maximum energy values used for AGC
    Qt3DCore::QCircularBuffer<float> m_lastRmsValues;  //!< list of last RMS values

    Qt3DCore::QCircularBuffer<float> m_spectralFluxHistory;
    Qt3DCore::QCircularBuffer<QColor> m_spectralColorHistory;
    QVector<float> m_spectralFluxNormalized;
    double m_currentSpectralFlux;
};

#endif // SPEECHINPUTANALYZER_H
