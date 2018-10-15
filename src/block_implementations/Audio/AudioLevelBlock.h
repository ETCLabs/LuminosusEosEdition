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

#ifndef AUDIOLEVELBLOCK_H
#define AUDIOLEVELBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"
#include "audio/AudioInputAnalyzer.h"
#include "utils.h"

#include <QPointer>
#include <QImage>


class AudioLevelBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(AudioInputAnalyzer* analyzer READ getAnalyzer NOTIFY inputChanged)
    Q_PROPERTY(double currentBand READ getCurrentBand WRITE setCurrentBand NOTIFY currentBandChanged)
    Q_PROPERTY(double upperBound READ getUpperBound WRITE setUpperBound NOTIFY upperBoundChanged)
    Q_PROPERTY(double bandwidth READ getBandwidth WRITE setBandwidth NOTIFY bandwidthChanged)
    Q_PROPERTY(bool agcEnabled READ getAgcEnabled WRITE setAgcEnabled NOTIFY agcEnabledChanged)
    Q_PROPERTY(double manualGain READ getManualGain WRITE setManualGain NOTIFY manualGainChanged)
    Q_PROPERTY(double manualGainNorm READ getManualGainNorm WRITE setManualGainNorm NOTIFY manualGainNormChanged)
    Q_PROPERTY(bool frequencyMode READ getFrequencyMode WRITE setFrequencyMode NOTIFY frequencyModeChanged)
    Q_PROPERTY(double peakPosition READ getPeakPosition NOTIFY peakPositionChanged)
    Q_PROPERTY(QImage spectrogram READ getSpectrogram NOTIFY spectrogramChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Audio Level";
        info.nameInUi = "Spectrum";
        info.category << "Sound2Light";
        info.availabilityRequirements = {AvailabilityRequirement::AudioInput};
        info.helpText = "Outputs the level of the loudest frequency within the selected range.\n"
                        "The top of the selected area defines the maximum expected level.\n\n"
                        "Frequency Mode: outputs the relative position (in the selected range) of "
                        "the lowest frequency that reaches the top of the selcted area.\n"
                        "This mode is useful if you know that there is only one lowest frequency peak "
                        "in this area and you want to follow its frequency and not its loudness.\n"
                        "If there is no peak higher than the top of the range the last value is held.";
        info.qmlFile = "qrc:/qml/Blocks/Audio/AudioLevelBlock.qml";
		info.complete<AudioLevelBlock>();
		return info;
    }

    explicit AudioLevelBlock(MainController* m_controller, QString m_uid);
    ~AudioLevelBlock();

    virtual void getAdditionalState(QJsonObject& state) const override;

    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void inputChanged();
    void currentBandChanged();
    void agcEnabledChanged();
    void upperBoundChanged();
    void bandwidthChanged();
    void frequencyModeChanged();
    void peakPositionChanged();
    void manualGainChanged();
    void manualGainNormChanged();
    void spectrogramChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    QVector<double> getSpectrumPoints();

    QString getInputName() const;
    void setInputByName(QString name);

    AudioInputAnalyzer* getAnalyzer() const { return m_analyzer; }

    double getCurrentBand() const { return m_currentBand; }
    void setCurrentBand(double value);

    double getUpperBound() const { return m_upperBound; }
    void setUpperBound(double value) { m_upperBound = limit(0.07, value, 1); emit upperBoundChanged(); }

    double getBandwidth() const { return m_bandwidth; }
    void setBandwidth(double value) { m_bandwidth = limit(0.01, value, 1); emit bandwidthChanged(); }

    bool getAgcEnabled() const { return m_agcEnabled; }
    void setAgcEnabled(bool value) { m_agcEnabled = value; emit agcEnabledChanged(); }

    double getManualGain() const { return m_manualGain; }
    void setManualGain(double value) { m_manualGain = limit(0.2, value, 8); emit manualGainChanged(); }

    double getManualGainNorm() const;
    void setManualGainNorm(double value);

    bool getFrequencyMode() const { return m_frequencyMode; }
    void setFrequencyMode(bool value) { m_frequencyMode = value; emit frequencyModeChanged(); }

    double getPeakPosition() const { return m_peakPosition; }

    QImage getSpectrogram() const {return m_spectrogram; }

    virtual void onControllerRotated(double relativeAmount, double, bool) override;

private:
    void updateOutputLevel();
    void updateOutputFrequency();

protected:
    QPointer<AudioInputAnalyzer> m_analyzer;

    double m_currentBand;  //!< lower end of band
    double m_upperBound;  //!< upper end of level range
    double m_bandwidth;  //!< bandwidth, upper end of band is currentBand + bandwidth

    double m_lastValue;
    double m_peakPosition;

    bool m_agcEnabled;
    double m_manualGain;

    bool m_frequencyMode;

    BoolAttribute m_showSpectrogram;

    QImage m_spectrogram;
    int m_spectrogramPos;

};

#endif // AUDIOLEVELBLOCK_H
