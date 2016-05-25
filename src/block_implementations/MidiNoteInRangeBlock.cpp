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

#include "MidiNoteInRangeBlock.h"

#include "MainController.h"
#include "NodeBase.h"


MidiNoteInRangeBlock::MidiNoteInRangeBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid, info().qmlFile)
	, m_key(60)
	, m_key2(61)
	, m_channel(1)
	, m_useDefaultChannel(true)
{
	connect(controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

QJsonObject MidiNoteInRangeBlock::getState() const {
	QJsonObject state;
	state["key"] = getKey();
	state["key2"] = getKey2();
	state["channel"] = getChannel();
	state["useDefaultChannel"] = getUseDefaultChannel();
	return state;
}

void MidiNoteInRangeBlock::setState(const QJsonObject &state) {
	setKey(state["key"].toInt());
	setKey2(state["key2"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiNoteInRangeBlock::onMidiMessage(MidiEvent event) {
	// from improbable to probable
	// check if this is a Note message and key is in range:
	// (midiTarget is key of Note messages)
	if ((event.type == MidiConstants::NOTE_ON
			|| event.type == MidiConstants::NOTE_OFF)
			&& event.target >= m_key
			&& event.target <= m_key2) {
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

void MidiNoteInRangeBlock::setKey(int value) {
	m_key = limit(0, value, 127);
	if (m_key > m_key2) {
		setKey2(m_key);
	}
	emit keyChanged();
}

void MidiNoteInRangeBlock::setTone(int value) {
	value = limit(0, value, 11);
	int octave = m_key / 12;
	int key = octave * 12 + value;
	m_key = limit(0, key, 127);
	if (m_key > m_key2) {
		setKey2(m_key);
	}
	emit keyChanged();
}

void MidiNoteInRangeBlock::setOctave(int value) {
	value = limit(0, value, 10);
	int tone = m_key % 12;
	int key = value * 12 + tone;
	m_key = limit(0, key, 127);
	if (m_key > m_key2) {
		setKey2(m_key);
	}
	emit keyChanged();
}

void MidiNoteInRangeBlock::setKey2(int value) {
	m_key2 = limit(0, value, 127);
	if (m_key2 < m_key) {
		setKey(m_key2);
	}
	emit key2Changed();
}

void MidiNoteInRangeBlock::setTone2(int value) {
	value = limit(0, value, 11);
	int octave = m_key2 / 12;
	int key = octave * 12 + value;
	m_key2 = limit(0, key, 127);
	if (m_key2 < m_key) {
		setKey(m_key2);
	}
	emit key2Changed();
}

void MidiNoteInRangeBlock::setOctave2(int value) {
	value = limit(0, value, 10);
	int tone = m_key2 % 12;
	int key = value * 12 + tone;
	m_key2 = limit(0, key, 127);
	if (m_key2 < m_key) {
		setKey(m_key2);
	}
	emit key2Changed();
}
