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

#include "MidiControlOutBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


MidiControlOutBlock::MidiControlOutBlock(MainController* controller, QString uid)
	: OneInputBlock(controller, uid)
	, m_target(1)
	, m_channel(1)
	, m_useDefaultChannel(true)
	, m_lastValue(0.0)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void MidiControlOutBlock::getAdditionalState(QJsonObject& state) const {
	state["target"] = getTarget();
	state["channel"] = getChannel();
    state["useDefaultChannel"] = getUseDefaultChannel();
}

void MidiControlOutBlock::setAdditionalState(const QJsonObject &state) {
	setTarget(state["target"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiControlOutBlock::onValueChanged() {
    double value = m_inputNode->getValue();
	if (value == m_lastValue) return;
	int type = MidiConstants::CONTROL_CHANGE;
	int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultOutputChannel() : m_channel;
	int target = m_target;
	m_controller->midi()->sendChannelVoiceMessage(type, channel, target, value);
	emit messageSent();
	m_lastValue = value;
}
