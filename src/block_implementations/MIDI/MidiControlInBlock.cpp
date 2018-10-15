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

#include "core/MainController.h"
#include "core/Nodes.h"


MidiControlInBlock::MidiControlInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid)
	, m_target(1)
	, m_channel(1)
    , m_useDefaultChannel(true)
    , m_learning(false)
{
	connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

void MidiControlInBlock::getAdditionalState(QJsonObject& state) const {
	state["target"] = getTarget();
	state["channel"] = getChannel();
    state["useDefaultChannel"] = getUseDefaultChannel();
}

void MidiControlInBlock::setAdditionalState(const QJsonObject &state) {
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

void MidiControlInBlock::startLearning() {
    // check if already in learning state:
    if (m_learning) {
        // yes -> cancel learning:
        m_controller->midi()->removeNextEventCallback(getUid());
        setLearning(false);
        return;
    }
    // listen for the next event:
    setLearning(true);
    m_controller->midi()->registerForNextEvent(getUid(), [this](MidiEvent event) { this->checkIfEventFits(event); });
}

void MidiControlInBlock::checkIfEventFits(MidiEvent event) {
    setLearning(false);
    // chek if this event was a control change event:
    if (event.type != MidiConstants::CONTROL_CHANGE) {
        m_controller->guiManager()->showToast("This was not a Control Change event.");
        return;
    }
    // set attributes to match the event:
    setUseDefaultChannel(false);
    setChannel(event.channel);
    setTarget(event.target);
    // update output:
    m_outputNode->setValue(event.value);
    emit validMessageReceived();
}
