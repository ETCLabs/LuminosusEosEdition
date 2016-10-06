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

#ifndef DELAYBLOCK_H
#define DELAYBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include <QTimer>


class DelayBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal onDelay READ getOnDelay WRITE setOnDelay NOTIFY onDelayChanged)
	Q_PROPERTY(qreal offDelay READ getOffDelay WRITE setOffDelay NOTIFY offDelayChanged)
	Q_PROPERTY(bool outputIsActive READ getOutputIsActive WRITE setOutputIsActive NOTIFY outputIsActiveChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Delay";
        info.category << "Logic";
        info.helpText = "If the incoming value is greater than 0 the output will be activated after "
						"the given On-Delay.\n"
                        "When it is 0 the output will be deactivated after "
						"the given Off-Delay.";
		info.qmlFile = "qrc:/qml/Blocks/DelayBlock.qml";
		info.complete<DelayBlock>();
		return info;
	}

	explicit DelayBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void onDelayChanged();
	void offDelayChanged();
	void outputIsActiveChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getOnDelay() const { return m_onDelay; }
	void setOnDelay(qreal value) { m_onDelay = limit(0, value, 999); emit onDelayChanged(); }

	qreal getOffDelay() const { return m_offDelay; }
	void setOffDelay(qreal value) { m_offDelay = limit(0, value, 999); emit offDelayChanged(); }

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

	// will be called after on delay time if no triggerOff happend
	// - calls sendOnSignal() and starts holdMaxTimer
	void onOnDelayEnd();

	// will be called after off delay time if no triggerOn happend
	// - calls sendOffSignal()
	void onOffDelayEnd();

protected:
	double		m_lastValue;

	qreal		m_onDelay;  // On delay in seconds
	qreal		m_offDelay;  // Off delay in seconds
	bool		m_outputIsActive;  // true if trigger is activated and not yet released

	QTimer		m_onDelayTimer;  // Timer object for On delay
	QTimer		m_offDelayTimer;  // Timer object for Off delay
};

#endif // DELAYBLOCK_H
