#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "utils.h"

#include <QObject>
#include <QMap>
#include <QPointer>

// forward declarations:
class MainController;
class AudioInputAnalyzer;
class SpeechInputAnalyzer;


/**
 * @brief The AudioEngine class is responsible for managing all AudioInputAnalyzer objects.
 */
class AudioEngine : public QObject
{

    Q_OBJECT

public:

    /**
     * @brief AudioEngine creates an AudioEngine object
     * @param controller a pointer to the main controller
     */
    explicit AudioEngine(MainController* controller);
    ~AudioEngine();

public slots:
    /**
     * @brief getDeviceNameList returns a list of all input device names
     * @return a list of names
     */
    QStringList getDeviceNameList() const;

    /**
     * @brief getDefaultAnalyzer returns the analyzer object of the default input device
     * @return a pointer to an AudioInputAnalyzer object
     */
    AudioInputAnalyzer* getDefaultAnalyzer() const;

    /**
     * @brief getAnalyzerByName return the analyzer for the input device with the given name,
     * if this device doesn't exists, a nullpointer is returned
     * @param name of the input device
     * @return a pointer to an AudioInputAnalyzer object or null
     */
    AudioInputAnalyzer* getAnalyzerByName(QString name) const;

    /**
     * @brief getDefaultSpeechAnalyzer returns the speech analyzer object of the default input device
     * @return a pointer to an SpeechInputAnalyzer object
     */
    SpeechInputAnalyzer* getDefaultSpeechAnalyzer() const;

    /**
     * @brief getSpeechAnalyzerByName returns the speech analyzer for the input device with the given name,
     * if this device doesn't exists, a nullpointer is returned
     * @param name of the input device
     * @return a pointer to an SpeechInputAnalyzer object or null
     */
    SpeechInputAnalyzer* getSpeechAnalyzerByName(QString name) const;

    /**
     * @brief getMaxLevelOfDevice returns the current level of an input device
     * @param name of the input device
     * @return a level between 0 and 1 or 0 if the device doesn't exist
     */
    double getMaxLevelOfDevice(QString name) const;

    // --------------------- Outputs --------------------

    QStringList getOutputNameList() const;

private:
    /**
     * @brief initInputs creates AudioInputAnalyzer and SpeechInputAnalyzer for all input devices
     */
    void initInputs();

protected:
    MainController* const m_controller;  //!< a pointer to the MainController
    /**
     * @brief m_audioInputs a map of device names and their AudioInputAnalyzer objects
     */
    QMap<QString, QPointer<AudioInputAnalyzer>> m_audioInputs;
    /**
     * @brief m_speechInputs a map of device names and their SpeechInputAnalyzer objects
     */
    QMap<QString, QPointer<SpeechInputAnalyzer>> m_speechInputs;
};

#endif // AUDIOENGINE_H
