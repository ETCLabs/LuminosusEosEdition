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

#ifndef EOSCHANBLOCK_H
#define EOSCHANBLOCK_H

#include "block_data/OneInputBlock.h"
#include "utils.h"


class EosChanBlock : public OneInputBlock
{
	Q_OBJECT

    Q_PROPERTY(int channelNumber READ getChannelNumber WRITE setChannelNumber NOTIFY channelNumberChanged)
    Q_PROPERTY(QString valueText READ getValueText NOTIFY valueTextChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Eos Channel";
		info.nameInUi = "Channel";
        info.category << "Eos";
        info.helpText = "Controls the selected channel of an Eos console.\n"
                        "Sets OSC user implicitly to 0.";
		info.qmlFile = "qrc:/qml/Blocks/EosChanBlock.qml";
		info.complete<EosChanBlock>();
		return info;
	}

	explicit EosChanBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void channelNumberChanged();
    void lastValueChanged();
    void valueTextChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onValueChanged();

	int getChannelNumber() const { return m_chanNumber; }
    void setChannelNumber(int value);

    QString getValueText() const { return m_valueText; }
    void setValueText(QString value) { m_valueText = value; emit valueTextChanged(); }

protected:
	int m_chanNumber;
    std::vector<double> m_lastValue;
    QString m_valueText;
};

#endif // EOSCHANBLOCK_H
