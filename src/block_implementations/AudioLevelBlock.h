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

#include "block_data/OneOutputBlock.h"
#include "AudioInputAnalyzer.h"
#include "utils.h"

#include <QPointer>


class AudioLevelBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(AudioInputAnalyzer* analyzer READ getAnalyzer NOTIFY inputChanged)
    Q_PROPERTY(double currentBand READ getCurrentBand WRITE setCurrentBand NOTIFY currentBandChanged)
    Q_PROPERTY(bool agcEnabled READ getAgcEnabled WRITE setAgcEnabled NOTIFY agcEnabledChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Audio Level";
        info.category << "Sound2Light";
        info.dependencies = {BlockDependency::AudioInput};
        info.helpText = "Outputs the level at the selected frequency (red stripe).\n"
                        "The input device can be chosen above.";
		info.qmlFile = "qrc:/qml/Blocks/AudioLevelBlock.qml";
		info.complete<AudioLevelBlock>();
		return info;
    }

    explicit AudioLevelBlock(MainController* m_controller, QString m_uid);
    ~AudioLevelBlock();

	virtual QJsonObject getState() const override;

    virtual void setState(const QJsonObject& state) override;

signals:
    void inputChanged();
    void currentBandChanged();
    void agcEnabledChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    QVector<double> getSpectrumPoints();

    QString getInputName() const;
    void setInputByName(QString name);

    AudioInputAnalyzer* getAnalyzer() const { return m_analyzer; }

    qreal getCurrentBand() const { return m_currentBand; }
    void setCurrentBand(double value);

    bool getAgcEnabled() const { return m_agcEnabled; }
    void setAgcEnabled(bool value) { m_agcEnabled = value; emit agcEnabledChanged(); }

protected:
    QPointer<AudioInputAnalyzer> m_analyzer;

    double m_currentBand;

    double m_lastValue;

    bool m_agcEnabled;

};

#endif // AUDIOLEVELBLOCK_H
