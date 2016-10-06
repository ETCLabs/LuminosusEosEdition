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

#include "DelayBlock.h"

#include "Nodes.h"
#include "MainController.h"  // for LuminosusConstants


DelayBlock::DelayBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid, info().qmlFile)
	, m_lastValue(0.0)
	, m_onDelay(0.0)
	, m_offDelay(0.0)
	, m_outputIsActive(false)
{
	// prepare Timers:
	m_onDelayTimer.setSingleShot(true);
	connect(&m_onDelayTimer, SIGNAL(timeout()), this, SLOT(onOnDelayEnd()));
	m_offDelayTimer.setSingleShot(true);
	connect(&m_offDelayTimer, SIGNAL(timeout()), this, SLOT(onOffDelayEnd()));

	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject DelayBlock::getState() const {
	QJsonObject state;
	state["onDelay"] = getOnDelay();
	state["offDelay"] = getOffDelay();
	return state;
}

void DelayBlock::setState(const QJsonObject &state) {
	setOnDelay(state["onDelay"].toDouble());
	setOffDelay(state["offDelay"].toDouble());
}

void DelayBlock::onValueChanged() {
    double value = m_inputNode->getValue();
	// check for trigger:
	if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
		// activate trigger:
		triggerOn();
	} else if (value < LuminosusConstants::triggerThreshold && m_lastValue >= LuminosusConstants::triggerThreshold) {
		// release trigger:
		triggerOff();
	}
	m_lastValue = value;

}

void DelayBlock::triggerOn() {
	// stop releaseDelayTimer if it is running:
	m_offDelayTimer.stop();

	// ignore triggerOn if output is still active:
	if (m_outputIsActive) return;

	// ignore triggerOn if onDelayTimer of previous triggerOn is still running:
	if (m_onDelayTimer.isActive()) return;

	// call onOnDelayEnd() after onDelay time:
	m_onDelayTimer.start(m_onDelay * 1000);
}

void DelayBlock::triggerOff() {
	// stop onDelayTimer if it is running:
	m_onDelayTimer.stop();

	// ignore triggerOff if output is not active:
	if (!m_outputIsActive) return;

	// ignore triggerOff if offDelayTimer of previous triggerOff is still running:
	if (m_offDelayTimer.isActive()) return;

	// call onOffDelayEnd() after offDelay time:
	m_offDelayTimer.start(m_offDelay * 1000);
}

void DelayBlock::onOnDelayEnd() {
	Q_ASSERT(!m_outputIsActive);

	setOutputIsActive(true);
	m_outputNode->setValue(1.0);
}

void DelayBlock::onOffDelayEnd() {
	Q_ASSERT(m_outputIsActive);

	setOutputIsActive(false);
	m_outputNode->setValue(0.0);
}
