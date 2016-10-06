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

#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "AudioInputAnalyzer.h"
#include "utils.h"

#include <QObject>
#include <QMap>
#include <QPointer>

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
     * @brief getMaxLevelOfDevice returns the current level of an input device
     * @param name of the input device
     * @return a level between 0 and 1 or 0 if the device doesn't exist
     */
    double getMaxLevelOfDevice(QString name) const;

private:
    /**
     * @brief initInputs creates AudioInputAnalyzer for all input devices
     */
    void initInputs();

protected:
    MainController* const m_controller;  //!< a pointer to the MainController
    /**
     * @brief m_audioInputs a map of device names and their AudioInputAnalyzer objects
     */
    QMap<QString, QPointer<AudioInputAnalyzer>> m_audioInputs;
};

#endif // AUDIOENGINE_H
