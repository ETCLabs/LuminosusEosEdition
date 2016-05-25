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

#include "OscOutBlock.h"

#include "MainController.h"
#include "NodeBase.h"


OscOutBlock::OscOutBlock(MainController *controller, QString uid)
	: OneInputBlock(controller, uid, info().qmlFile)
	, m_message("")
	, m_lastValue(0)
	, m_minValue(0)
	, m_maxValue(1)
	, m_useInteger(false)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject OscOutBlock::getState() const {
	QJsonObject state;
	state["message"] = getMessage();
	state["minValue"] = getMinValue();
	state["maxValue"] = getMaxValue();
	state["useInteger"] = getUseInteger();
	return state;
}

void OscOutBlock::setState(const QJsonObject &state) {
	setMessage(state["message"].toString());
	setMinValue(state["minValue"].toDouble());
	setMaxValue(state["maxValue"].toDouble());
	setUseInteger(state["useInteger"].toBool());
}

void OscOutBlock::onValueChanged() {
	double value = m_inputNode->data->getValue();
	if (value == m_lastValue) return;
	QString message = m_message;
	if (message.isEmpty()) return;
	if (message.contains("<value>")) {
		double scaledValue = m_minValue + (value * (m_maxValue - m_minValue));
		if (m_useInteger) {
			message.replace("<value>", QString::number(int(scaledValue)));
		} else {
			message.replace("<value>", QString::number(scaledValue, 'g', 4));
		}
		m_controller->osc()->sendMessage(message);
		emit messageSent();
	} else if (value >= 0.5 && m_lastValue < 0.5) {
		m_controller->osc()->sendMessage(message);
		emit messageSent();
	}
	m_lastValue = value;
}

void OscOutBlock::setMinValue(double value) {
	if (value < 0) {
		value = 0;
	}
	m_minValue = value;
	if (m_maxValue <= m_minValue) {
		setMaxValue(m_minValue + 1);
	}
	emit minValueChanged();
}

void OscOutBlock::setMaxValue(double value) {
	if (value <= 0) {
		value = 1;
	}
	m_maxValue = value;
	if (m_minValue >= m_maxValue) {
		setMinValue(m_maxValue - 1);
	}
	emit maxValueChanged();
}
