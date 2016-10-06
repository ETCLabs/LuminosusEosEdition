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

#include "MidiNoteOutBlock.h"

#include "MainController.h"
#include "Nodes.h"


MidiNoteOutBlock::MidiNoteOutBlock(MainController *controller, QString uid)
	: OneInputBlock(controller, uid, info().qmlFile)
	, m_key(60)
	, m_channel(1)
	, m_useDefaultChannel(true)
	, m_lastValue(0.0)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject MidiNoteOutBlock::getState() const {
	QJsonObject state;
	state["key"] = getKey();
	state["channel"] = getChannel();
	state["useDefaultChannel"] = getUseDefaultChannel();
	return state;
}

void MidiNoteOutBlock::setState(const QJsonObject &state) {
	setKey(state["key"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiNoteOutBlock::onValueChanged() {
    double value = m_inputNode->getValue();
	if (value == m_lastValue) return;
	int type = MidiConstants::NOTE_ON;
	int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultOutputChannel() : m_channel;
	int target = m_key;
	m_controller->midi()->sendChannelVoiceMessage(type, channel, target, value);
	emit messageSent();
	m_lastValue = value;
}

void MidiNoteOutBlock::setTone(int value) {
	value = limit(0, value, 11);
	int octave = m_key / 12;
	int key = octave * 12 + value;
	m_key = limit(0, key, 127);
	emit keyChanged();
}

void MidiNoteOutBlock::setOctave(int value) {
	value = limit(0, value, 10);
	int tone = m_key % 12;
	int key = value * 12 + tone;
	m_key = limit(0, key, 127);
	emit keyChanged();
}

