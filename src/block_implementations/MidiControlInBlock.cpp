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

#include "MidiControlInBlock.h"

#include "MainController.h"
#include "NodeBase.h"


MidiControlInBlock::MidiControlInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid, info().qmlFile)
	, m_target(1)
	, m_channel(1)
	, m_useDefaultChannel(true)
{
	connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

QJsonObject MidiControlInBlock::getState() const {
	QJsonObject state;
	state["target"] = getTarget();
	state["channel"] = getChannel();
	state["useDefaultChannel"] = getUseDefaultChannel();
	return state;
}

void MidiControlInBlock::setState(const QJsonObject &state) {
	setTarget(state["target"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiControlInBlock::onMidiMessage(MidiEvent event) {
	// from improbable to probable
	// check if this is a ControlChange message and target is correct:
	if (event.type == MidiConstants::CONTROL_CHANGE
			&& event.target == m_target) {
		// check if channel is correct:
		int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultInputChannel() : m_channel;
		if (channel == MidiConstants::OMNI_MODE_CHANNEL || event.channel == channel) {
			// channel and target are correct
			// set value:
			m_outputNode->setValue(event.value);
			emit validMessageReceived();
		}
	}
}
