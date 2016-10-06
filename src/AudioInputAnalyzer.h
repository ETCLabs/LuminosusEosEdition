#ifndef AUDIOINPUTANALYZER_H
#define AUDIOINPUTANALYZER_H

#include "utils.h"
#include "ffft/FFTRealFixLen.h"

#include "QCircularBuffer.h"
#include <QObject>
#include <QtMath>
#include <QMap>
#include <QSet>
#include <QPointer>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QLinkedList>
#include <QColor>

#include <vector>


// --------- Constants for Spectrum Creation -----------

// audio input sampling rate
static const int AUDIO_SAMPLING_RATE = 44100;  // Hz

// length of circular input buffer in samples
static const int CIRC_BUFFER_LENGTH = AUDIO_SAMPLING_RATE * 0.3;  // 0.3s

// power of two of LONG_NUM_SAMPLES
static const int LONG_NUM_SAMPLES_EXPONENT = 12;

// number of samples to use for the "long" bass spectrum
static const int LONG_NUM_SAMPLES = qPow(2, LONG_NUM_SAMPLES_EXPONENT);  // 12 -> 4096

// power of two of SHORT_NUM_SAMPLES
static const int SHORT_NUM_SAMPLES_EXPONENT = 11;

// number of samples to use for the "short" mid-high spectrum
static const int SHORT_NUM_SAMPLES = qPow(2, SHORT_NUM_SAMPLES_EXPONENT);  // 11 -> 2048

// number of data points for the simplified spectrum
static const int SIMPLIFIED_SPECTRUM_LENGTH = 128;


// --------- Constants for Onset, Spectral Flux and BPM Detection -----------

// rate to calculate the spectrum and spectral flux
// "hopSize" (SAMPLES_BETWEEN_SPECTRUM_UPDATES) is based on this value
static const int SPECTRUM_UPDATE_RATE = 50;  // Hz

// aka "hopSize" or frames to skip between spectrum calculations
// should be smaller tha length of FFT window so that the windows are overlapping
static const int SAMPLES_BETWEEN_SPECTRUM_UPDATES = AUDIO_SAMPLING_RATE / SPECTRUM_UPDATE_RATE;

// number of simplified spectrums to keep in the history
static const int SPECTRUM_HISTORY_LENGTH = 10;

// number of spectral flux values to keep in the history for Onset detection
static const int SPECTRAL_FLUX_HISTORY_LENGTH = SPECTRUM_UPDATE_RATE * 5;  // 5s

// Spectrums, Spectral Flux and Onsets will be updated when new input data is available
// so this value is the approx. rate audioDataReady() is called
static const int BPM_UPDATE_RATE = 20; // Hz

// BPM tempo detection will be only run every n-th call
static const int CALLS_TO_WAIT_FOR_BPM = 5;

// the seconds of BPMs to store to allow smoothing of the output
static const int SECONDS_OF_INTERVALS_TO_STORE = 4;

// the actual number of BPMs to store to allow smoothing of the output
static const int INTERVALS_TO_STORE = SECONDS_OF_INTERVALS_TO_STORE * BPM_UPDATE_RATE / CALLS_TO_WAIT_FOR_BPM;


// maximum absolute value in the result of the FFT
// estimated from previous tests (exponent/samples: max value):
// 11/2048: 51, 12/4096: 96, 13/8192: 195, 14/16384: 350
static const int LONG_MAX_FFT_VALUE = 1;
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

// ---------------------------------------------------

class MainController;  // forward declaration

// A class to modell a cluster of Inter Offset Intervalls (IOIs).
// defined in .cpp file
class BeatAgent;


// ---------------------------------- AudioInputAnalyzer --------------------------------

/**
 * @brief The AudioInputAnalyzer class is responsible for analyzing a single
 * audio input device using the Fast Fourier Transform (FFT) to generate a frequency spectrum.
 *
 * It uses a "long" sample buffer to calculate FFT of low, bass frequencies and a shorter
 * sample bufffer for calculating the FFT for mid and high frequencies.
 *
 * It also calculates the spectral flux and detects onsets in the input stream.
 * Based on the onsets it calculates a BPM tempo value.
 */
class AudioInputAnalyzer : public QObject {

    Q_OBJECT

    Q_PROPERTY(float bpm READ getBpm NOTIFY bpmChanged)
    Q_PROPERTY(bool bpmIsValid READ getBpmIsValid NOTIFY bpmChanged)

public:

    /**
     * @brief AudioInputAnalyzer creates an object of this class
     * @param inputInfo containing the information about the device to analyze
     * @param controller a pointer to the MainController
     */
    explicit AudioInputAnalyzer(QAudioDeviceInfo inputInfo, MainController* controller);
    ~AudioInputAnalyzer();

    /**
     * @brief AudioInputAnalyzer Used to create an empty / dummy analyzer.
     * @param controller a pointer to the MainController
     */
    explicit AudioInputAnalyzer(MainController* controller);

    /**
     * @brief addReference registers an object as "interested in the results" of this analyzer
     *
     * This analyzer only starts analyzing when at least one object is registered
     * (reference counting).
     *
     * @param ref a pointer to the interested object
     */
    void addReference(void* ref);

    /**
     * @brief removeReference unregisters an interested object
     * @param ref a pointer to an object previously registered with addReference()
     */
    void removeReference(void* ref);

signals:
    /**
     * @brief bpmChanged is emitted when a new BPM value was detected
     */
    void bpmChanged();

    /**
     * @brief spectralFluxHistoryChanged is emitted when the spectral flux history changed
     */
    void spectralFluxHistoryChanged();

    /**
     * @brief spectrumChanged is emitted when the spectrum changed
     */
    void spectrumChanged();

public slots:
    /**
     * @brief getSimplifiedSpectrum returns a simplified scaled frequency spectrum
     * @return spectrum with energy values between 0 and 1
     */
    const std::vector<double>& getSimplifiedSpectrum() const { return m_simplifiedSpectrum; }

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
     * @brief getOnsets returns the last detected onsets. The returned array matches the one
     * returned by getSpectralFluxHistory(). For each spectral flux value it contains either
     * true if an onset was detected or false if not.
     * @return array of bools whichs indexes match those of getSpectralFluxHistory()
     */
    const QVector<bool>& getOnsets() const { return m_onsetBuffer; }

    /**
     * @brief getDetectedOnsets returns the detected onsets
     * @return an array with positions relative to the spectral flux history [0...1]
     */
    QVector<double> getDetectedOnsets() const { return m_detectedOnsets; }

    /**
     * @brief getMaxLevel returns maximum level of all frequencies
     * @return a level between 0 and 1
     */
    float getMaxLevel() const { return m_maxLevel; }

    /**
     * @brief getLevelAtBand return the level of a certain frequency bin
     * @param band the "band" between 0 and 1
     * @return a level between 0 and 1
     */
    float getLevelAtBand(double band) const;

    /**
     * @brief getDeviceName returns the name of the device that is analyzed
     * @return an input device name
     */
    QString getDeviceName() const { return m_deviceName; }

    /**
     * @brief getBpm returns the last detected BPM value
     * @param minBpm the minimum expected BPM [0=auto, 50, 75, 100, 150]
     * @return a BPM value [50...300]
     */
    float getBpm(int minBpm=75) const { return bpmInRange(m_bpm, minBpm); }
    /**
     * @brief getBpmIsValid returns if the BPM value is new and value or if it is too old
     * @return true if value was updated in the last 5s, false if it is too old or not valid
     */
    bool getBpmIsValid() const { return HighResTime::elapsedSecSince(m_lastBpmDetection) < 5; }

    /**
     * @brief getNewSpectrumCount returns the count of new, unused spectrums,
     * will be modified by update updateSpectrum() calls
     * @return count of new spectrums
     */
    int getNewSpectrumCount() const { return m_newSpectrumCount; }

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
     * @brief setBpm sets the detected BPM value
     * @param value a BPM tempo value
     */
    void setBpm(float value);

    static float bpmInRange(float bpm, const int minBPM);

private slots:

    void audioDataReady();

    void updateSpectrum();

    void analyzeNewSamples();

    void createSpectrumTillIndex(int endIndex);

    void updateRawSpectrumsTillIndex(int endIndex);

    void createSimplifiedSpectrumFromRawSpectrums();

    void updateOnsets();

    /**
     * @brief updateAgents categorizes intervalls between the onsets into clusters
     */
    void updateAgents();

    /**
     * @brief plausibleAgentForInterval is a helper function for the evaluation
     * @param interval interval to check for
     * @param maxScore maximums interval score
     * @return a plausible BeatAgent or nullptr if no one was found
     */
    BeatAgent* plausibleAgentForInterval(float interval, float maxScore);

    /**
     * @brief evaluateAgents finds the cluster with the highest score and updates BPM value
     */
    void evaluateAgents();

    /**
     * @brief updateAGC calculates the required gain and changes the actual gain in small steps
     * based on the last maximum values of the FFT
     */
    void updateAGC();

protected:
    MainController* const m_controller;  //!< a pointer to the main controller

    QSet<void*> m_referenceList;  //!< list of registered objects

    QPointer<QAudioInput> m_audioInput;  //!< audio input device
    QPointer<QIODevice> m_audioRecordDevice;  //!< audio record device
    QAudioFormat m_audioFormat;  //!< format to record audio in
    QString m_deviceName;  //!< name of input device
    Qt3DCore::QCircularBuffer<float> m_circBuffer;  //!< ring buffer to store incoming audio samples
    int m_analyzed_till_index;  //!< the end index of the last analyzed FFT window in circular buffer

    ffft::FFTRealFixLen<LONG_NUM_SAMPLES_EXPONENT> m_longFftreal;  //!< object for FFT calculations
    std::vector<float> m_longBuffer;  //!< buffer for long FFT
    std::vector<float> m_longWindow;  //!< window function array for long FFT
    std::vector<float> m_longFftOutput;  //!< output buffer for long FFT
    std::vector<float> m_longSpectrum;  //!< resulting spectrum of long FFT

    ffft::FFTRealFixLen<SHORT_NUM_SAMPLES_EXPONENT> m_shortFftreal;  //!< object for FFT calculations
    std::vector<float> m_shortBuffer;  //!< buffer for short FFT
    std::vector<float> m_shortWindow;  //!< window function array for short FFT
    std::vector<float> m_lastShortFftOutput;  //!< previous output buffer for short FFT
    std::vector<float> m_shortFftOutput;  //!< output buffer for short FFT
    std::vector<float> m_shortSpectrum;  //!< resulting spectrum of short FFT

    Qt3DCore::QCircularBuffer<std::vector<double>> m_spectrumHistory;  //!< last spectrums
    int m_newSpectrumCount;  //!< count of new, unused spectrums in spectrum history
    std::vector<double> m_simplifiedSpectrum;  //!< the current simplified spectrum to display

    float m_maxLevel;  //!< maximum level of input device

    float m_agcValue;  //!< best amplifictation factor of spectrum
    float m_spectralFluxAgcValue;  //!< best amplifictation factor for spectral flux
    float m_compression;  //!< Compression factor (the higher it is the more spectrum gets compressed)
    Qt3DCore::QCircularBuffer<float> m_lastMaxValues;  //!< list of last maximum energy values used for AGC

    Qt3DCore::QCircularBuffer<float> m_spectralFluxHistory;
    Qt3DCore::QCircularBuffer<QColor> m_spectralColorHistory;
    QVector<float> m_spectralFluxNormalized;
    double m_currentSpectralFlux;
    QVector<bool> m_onsetBuffer;
    QVector<double> m_detectedOnsets;

    float m_bpm;
    int m_callsSinceLastBpmUpdate;
    HighResTime::time_point_t m_lastBpmDetection;

    QLinkedList<BeatAgent> m_agents; //!< the IOI Clusters identified from the intervalls
    Qt3DCore::QCircularBuffer<float> m_lastIntervals; //!< the last bpm values stored as their interval, to achieve smoothing
};

#endif // AUDIOINPUTANALYZER_H
