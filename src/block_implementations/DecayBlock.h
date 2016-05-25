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

#ifndef DECAYBLOCK_H
#define DECAYBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include <QTimer>


class DecayBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal decay READ getDecay WRITE setDecay NOTIFY decayChanged)
	Q_PROPERTY(bool outputIsActive READ getOutputIsActive WRITE setOutputIsActive NOTIFY outputIsActiveChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Decay";
		info.category = QStringList {"Logic"};
		info.helpText = "If the incoming value is above 50% the output will be activated.\n"
						"It will be deactivated when the incoming value falls below 50% "
						"or after the given time (decay).";
		info.qmlFile = "qrc:/qml/Blocks/DecayBlock.qml";
		info.complete<DecayBlock>();
		return info;
	}

	explicit DecayBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void decayChanged();
	void outputIsActiveChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getDecay() const { return m_decay; }
	void setDecay(qreal value) { m_decay = limit(0, value, 999); emit decayChanged(); }

	bool getOutputIsActive() const { return m_outputIsActive; }
	void setOutputIsActive(bool value) { m_outputIsActive = value; emit outputIsActiveChanged(); }


protected:
	// to be called when the trigger from the raw signal is activated
	// - starts onDelayTimer
	void triggerOn();
	// to be called when the trigger from the raw signal is released
	// - starts offDelayTimer
	void triggerOff();

private slots:
	void onValueChanged();

	void onDecayEnd();

protected:
	double		m_lastValue;

	qreal		m_decay;
	bool		m_outputIsActive;  // true if trigger is activated and not yet released

	QTimer		m_decayTimer;
};

#endif // DECAYBLOCK_H
