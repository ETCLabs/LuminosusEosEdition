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

#include "core/MainController.h"
#include "core/Nodes.h"


MidiProgramInBlock::MidiProgramInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid)
    , m_program(1)
	, m_channel(1)
	, m_useDefaultChannel(true)
	, m_programIsActive(false)
    , m_learning(false)
{
	connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
}

void MidiProgramInBlock::getAdditionalState(QJsonObject& state) const {
    state["program"] = getProgram();
	state["channel"] = getChannel();
    state["useDefaultChannel"] = getUseDefaultChannel();
}

void MidiProgramInBlock::setAdditionalState(const QJsonObject &state) {
    setProgram(state["program"].toInt());
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
            if (event.target == (m_program - 1)) {
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

void MidiProgramInBlock::setProgram(int value) {
    if (value == m_program) return;
    m_program = limit(1, value, 128);
	m_outputNode->setValue(0.0);
	m_programIsActive = false;
	emit programIsActiveChanged();
    emit programChanged();
}

void MidiProgramInBlock::startLearning() {
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

void MidiProgramInBlock::checkIfEventFits(MidiEvent event) {
    setLearning(false);
    // chek if this event was a control change event:
    if (event.type != MidiConstants::PROGRAM_CHANGE) {
        m_controller->guiManager()->showToast("This was not a Program Change event.");
        return;
    }
    // set attributes to match the event:
    setUseDefaultChannel(false);
    setChannel(event.channel);
    setProgram(event.target + 1);
    // update output:
    m_outputNode->setValue(1.0);
    emit validMessageReceived();
    m_programIsActive = true;
    emit programIsActiveChanged();
}
