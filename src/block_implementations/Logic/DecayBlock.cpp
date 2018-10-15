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

#include "DecayBlock.h"

#include "core/Nodes.h"
#include "core/MainController.h"  // for LuminosusConstants


DecayBlock::DecayBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid)
	, m_lastValue(0.0)
	, m_decay(0.1)
	, m_outputIsActive(false)
{
	// prepare Timers:
	m_decayTimer.setSingleShot(true);
	connect(&m_decayTimer, SIGNAL(timeout()), this, SLOT(onDecayEnd()));

	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void DecayBlock::getAdditionalState(QJsonObject& state) const {
    state["decay"] = getDecay();
}

void DecayBlock::setAdditionalState(const QJsonObject &state) {
	setDecay(state["decay"].toDouble());
}

void DecayBlock::onValueChanged() {
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

void DecayBlock::triggerOn() {
    if (m_outputIsActive) return;

	setOutputIsActive(true);
	m_outputNode->setValue(1.0);

	if (m_decay > 0) {
		m_decayTimer.start(m_decay * 1000);
	}
}

void DecayBlock::triggerOff() {
	// ignore triggerOff when output is already off:
	if (!m_outputIsActive) return;

	setOutputIsActive(false);
	m_outputNode->setValue(0.0);

	// if decayTimer is running, stop it:
	m_decayTimer.stop();
}

void DecayBlock::onDecayEnd() {
    if (!m_outputIsActive) return;

	setOutputIsActive(false);
	m_outputNode->setValue(0.0);
}
