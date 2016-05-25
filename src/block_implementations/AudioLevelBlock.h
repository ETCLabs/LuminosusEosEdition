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

#include <QObject>
#include <QTimer>
#include "block_data/OneOutputBlock.h"
#include "utils.h"


class AudioInputAnalyzer;


class AudioLevelBlock : public OneOutputBlock {

    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Audio Level";
		info.category = QStringList {"Sound2Light"};
		info.qmlFile = "qrc:/qml/Blocks/AudioLevelBlock.qml";
		info.complete<AudioLevelBlock>();
		return info;
	}

    double currentBand = 0.5;
    AudioInputAnalyzer* analyzer = nullptr;

    explicit AudioLevelBlock(MainController* m_controller, QString m_uid);
    ~AudioLevelBlock();

	virtual QJsonObject getState() const override;

    virtual void setState(const QJsonObject& state) override {
        setInputByName(state["inputName"].toString());
	}

signals:
    void inputChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void setInputByName(QString name);
    QString getInputName();
    void refreshOutput();
    QList<qreal> getSpectrumPoints();
    void setCurrentBand(double value) { currentBand = limitToOne(value); }
    qreal getCurrentBand() { return currentBand; }

};

#endif // AUDIOLEVELBLOCK_H
