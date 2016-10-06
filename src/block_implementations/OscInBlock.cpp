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

#include "OscInBlock.h"

#include "MainController.h"
#include "Nodes.h"


OscInBlock::OscInBlock(MainController *controller, QString uid)
	: OneOutputBlock(controller, uid, info().qmlFile)
	, m_message("")
	, m_minValue(0)
	, m_maxValue(1)
{
    connect(m_controller, SIGNAL(sendCustomOscToEosChanged()), this, SLOT(updateConnection()));
    if (m_controller->getSendCustomOscToEos()) {
        connect(controller->eosConnection(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
    } else {
        connect(controller->osc(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
    }
}

QJsonObject OscInBlock::getState() const {
	QJsonObject state;
	state["message"] = getMessage();
	state["minValue"] = getMinValue();
	state["maxValue"] = getMaxValue();
	return state;
}

void OscInBlock::setState(const QJsonObject &state) {
	setMessage(state["message"].toString());
	setMinValue(state["minValue"].toDouble());
	setMaxValue(state["maxValue"].toDouble());
}

void OscInBlock::onMessageReceived(OSCMessage msg) {
	if (msg.pathString() == m_message) {
		emit validMessageReceived();
		if (msg.arguments().size() == 0) {
			m_outputNode->setValue(1.0);
			QTimer::singleShot(100, this, SLOT(onEndOfPulse()));
		} else {
			double value = (msg.value() - m_minValue) / (m_maxValue - m_minValue);
			value = limit(0, value, 1);
			m_outputNode->setValue(value);
		}
	}
}

void OscInBlock::onEndOfPulse() {
    m_outputNode->setValue(0.0);
}

void OscInBlock::updateConnection() {
    if (m_controller->getSendCustomOscToEos()) {
        disconnect(m_controller->osc(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
        connect(m_controller->eosConnection(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
    } else {
        disconnect(m_controller->eosConnection(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
        connect(m_controller->osc(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
    }
}

void OscInBlock::setMinValue(double value) {
	if (value < 0) {
		value = 0;
	}
	m_minValue = value;
	if (m_maxValue <= m_minValue) {
		setMaxValue(m_minValue + 1);
	}
	emit minValueChanged();
}

void OscInBlock::setMaxValue(double value) {
	if (value <= 0) {
		value = 1;
	}
	m_maxValue = value;
	if (m_minValue >= m_maxValue) {
		setMinValue(m_maxValue - 1);
	}
	emit maxValueChanged();
}

