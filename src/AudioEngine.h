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

#include "utils.h"
#include "ffft/FFTRealFixLen.h"

#include "QCircularBuffer.h"
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QObject>
#include <QtMath>
#include <QDebug>

#include <algorithm>
#include <set>
#include <vector>

#define LONG_NUM_SAMPLES_EXPONENT 12
#define LONG_NUM_SAMPLES 4096
#define SHORT_NUM_SAMPLES_EXPONENT 10
#define SHORT_NUM_SAMPLES 1024
#define SIMPLIFIED_SPECTRUM_LENGTH 128


class MainController;  // forward declaration


class AudioInputAnalyzer : public QObject {
    Q_OBJECT

public:
    MainController* controller;
    std::set<void*> referenceList;

    QAudioInput* audioInput;
    QIODevice* audioIODevice;
    QAudioFormat audioFormat;
    QString deviceName;
	Qt3DCore::QCircularBuffer<float> circBuffer;

    ffft::FFTRealFixLen<LONG_NUM_SAMPLES_EXPONENT> longFftreal;
    std::vector<float> longBuffer;
    std::vector<float> longWindow;
    std::vector<float> longFftOutput;
    std::vector<float> longSpectrum;

    ffft::FFTRealFixLen<SHORT_NUM_SAMPLES_EXPONENT> shortFftreal;
    std::vector<float> shortBuffer;
    std::vector<float> shortWindow;
    std::vector<float> shortFftOutput;
    std::vector<float> shortSpectrum;

    float maxLevel = 0.0;
    std::vector<double> simplifiedSpectrum;

    explicit AudioInputAnalyzer(QAudioDeviceInfo inputInfo, MainController* controller) : QObject((QObject*)controller)
      , controller(controller)
      , deviceName(inputInfo.deviceName())
      , circBuffer(LONG_NUM_SAMPLES)
      , longBuffer(LONG_NUM_SAMPLES)
      , longWindow(LONG_NUM_SAMPLES)
      , longFftOutput(LONG_NUM_SAMPLES)
      , longSpectrum(LONG_NUM_SAMPLES / 2)
      , shortBuffer(SHORT_NUM_SAMPLES)
      , shortWindow(SHORT_NUM_SAMPLES)
      , shortFftOutput(SHORT_NUM_SAMPLES)
      , shortSpectrum(SHORT_NUM_SAMPLES / 2)
      , simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    {
        calculateWindows();
        initAudio(inputInfo);
    }

    /**
     * @brief AudioInputAnalyzer Used to create an empty / dummy analyzer.
     * @param controller
     */
    explicit AudioInputAnalyzer(MainController* controller) : QObject((QObject*)controller)
      , controller(controller)
      , deviceName("No Input")
      , circBuffer(LONG_NUM_SAMPLES)
      , simplifiedSpectrum(SIMPLIFIED_SPECTRUM_LENGTH)
    { }

    void addReference(void* ref);

    void removeReference(void* ref);

private:
    void calculateWindows();
    void initAudio(QAudioDeviceInfo info);

private slots:
    void audioDataReady();
    void updateSpectrum();
    void updateSimplifiedSpectrum();

signals:

public slots:
    std::vector<double>& getSimplifiedSpectrum() {
        return simplifiedSpectrum;
    }

    float getMaxLevel() {
        return maxLevel;
    }

    float getLevelAtBand(double band) {
        return simplifiedSpectrum[band*(simplifiedSpectrum.size()-1)];
    }
};


class AudioEngine : public QObject
{
    Q_OBJECT
public:
    MainController* controller;
    std::map<QString, AudioInputAnalyzer*> audioInputs;

    explicit AudioEngine(MainController* controller) : QObject((QObject*)controller), controller(controller) {
        initInputs();
    }

    ~AudioEngine() {
        for (auto entry: audioInputs) {
            if (entry.second) {
                entry.second->deleteLater();
                audioInputs[entry.first] = nullptr;
            }
        }
    }

private:
    void initInputs();

signals:

public slots:
    std::map<QString, AudioInputAnalyzer*> getInputs() {
        return audioInputs;
    }
    QStringList getInputList() {
        QStringList list;
        for (auto entry: audioInputs) {
            list.append(QString(entry.first));
        }
        return list;
    }

    AudioInputAnalyzer* getDefaultInput() {
        return audioInputs.begin()->second;
    }

    qreal getMaxLevelByName(QString name) {
        if (!mapContains(audioInputs, name)) {
            return 0.0;
        }
        return audioInputs[name]->getMaxLevel();
    }
};

#endif // AUDIOENGINE_H
