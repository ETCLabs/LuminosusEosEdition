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

#include "MidiNoteInBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


MidiNoteInBlock::MidiNoteInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid)
	, m_key(60)
	, m_channel(1)
	, m_useDefaultChannel(true)
    , m_learning(false)
{
	connect(controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

void MidiNoteInBlock::getAdditionalState(QJsonObject& state) const {
	state["key"] = getKey();
	state["channel"] = getChannel();
    state["useDefaultChannel"] = getUseDefaultChannel();
}

void MidiNoteInBlock::setAdditionalState(const QJsonObject &state) {
	setKey(state["key"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiNoteInBlock::onMidiMessage(MidiEvent event) {
	// from improbable to probable
	// check if this is a Note message and key is correct:
	// (midiTarget is key of Note messages)
	if ((event.type == MidiConstants::NOTE_ON
			|| event.type == MidiConstants::NOTE_OFF)
			&& event.target == m_key) {
		// check if channel is correct:
		int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultInputChannel() : m_channel;
		if (channel == MidiConstants::OMNI_MODE_CHANNEL || event.channel == channel) {
			// channel and key are correct
			// set value:
			m_outputNode->setValue(event.value);
			emit validMessageReceived();
		}
	}
}

void MidiNoteInBlock::setTone(int value) {
	value = limit(0, value, 11);
	int octave = m_key / 12;
	int key = octave * 12 + value;
	m_key = limit(0, key, 127);
	emit keyChanged();
}

void MidiNoteInBlock::setOctave(int value) {
	value = limit(0, value, 10);
	int tone = m_key % 12;
	int key = value * 12 + tone;
	m_key = limit(0, key, 127);
	emit keyChanged();
}

void MidiNoteInBlock::startLearning() {
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

void MidiNoteInBlock::checkIfEventFits(MidiEvent event) {
    setLearning(false);
    // chek if this event was a control change event:
    if (event.type != MidiConstants::NOTE_ON) {
        m_controller->guiManager()->showToast("This was not a Note event.");
        return;
    }
    // set attributes to match the event:
    setUseDefaultChannel(false);
    setChannel(event.channel);
    setKey(event.target);
    // update output:
    m_outputNode->setValue(event.value);
    emit validMessageReceived();
}
