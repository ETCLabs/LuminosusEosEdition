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

#include "MidiProgramInBlock.h"

#include "MainController.h"
#include "NodeBase.h"


MidiProgramInBlock::MidiProgramInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid, info().qmlFile)
	, m_target(1)
	, m_channel(1)
	, m_useDefaultChannel(true)
	, m_programIsActive(false)
{
	connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

QJsonObject MidiProgramInBlock::getState() const {
	QJsonObject state;
	state["target"] = getTarget();
	state["channel"] = getChannel();
	state["useDefaultChannel"] = getUseDefaultChannel();
	return state;
}

void MidiProgramInBlock::setState(const QJsonObject &state) {
	setTarget(state["target"].toInt());
	setChannel(state["channel"].toInt());
	setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void MidiProgramInBlock::onMidiMessage(MidiEvent event) {
	// from improbable to probable
	// check if this is a ProgramChange message:
	if (event.type == MidiConstants::PROGRAM_CHANGE) {
		// check if channel is correct:
		int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultInputChannel() : m_channel;
		if (channel == MidiConstants::OMNI_MODE_CHANNEL || event.channel == channel) {
			// channel and type are correct

			// check if program number (target) is correct:
			if (event.target == (m_target - 1)) {
				// it is correct -> activate output:
				m_outputNode->setValue(1.0);
				emit validMessageReceived();
				m_programIsActive = true;
				emit programIsActiveChanged();
			} else {
				// it is different -> deactivate output:
				m_outputNode->setValue(0.0);
				m_programIsActive = false;
				emit programIsActiveChanged();
			}
		}
	}
}

void MidiProgramInBlock::setTarget(int value) {
	if (value == m_target) return;
	m_target = limit(1, value, 128);
	m_outputNode->setValue(0.0);
	m_programIsActive = false;
	emit programIsActiveChanged();
	emit targetChanged();
}
