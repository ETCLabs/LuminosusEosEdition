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

#include "core/MainController.h"
#include "core/Nodes.h"


OscOutBlock::OscOutBlock(MainController *controller, QString uid)
	: OneInputBlock(controller, uid)
	, m_message("")
	, m_lastValue(0)
	, m_minValue(0)
	, m_maxValue(1)
	, m_useInteger(false)
    , m_negativeMinValue(this, "negativeMinValue", false)
    , m_negativeMaxValue(this, "negativeMaxValue", false)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void OscOutBlock::getAdditionalState(QJsonObject& state) const {
	state["message"] = getMessage();
	state["minValue"] = getMinValue();
	state["maxValue"] = getMaxValue();
    state["useInteger"] = getUseInteger();
}

void OscOutBlock::setAdditionalState(const QJsonObject &state) {
	setMessage(state["message"].toString());
	setMinValue(state["minValue"].toDouble());
	setMaxValue(state["maxValue"].toDouble());
	setUseInteger(state["useInteger"].toBool());
}

void OscOutBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    double absoluteValue = m_inputNode->getAbsoluteValue();
    if (absoluteValue == m_lastValue) return;
	QString message = m_message;
	if (message.isEmpty()) return;
	if (message.contains("<value>")) {
        double scaledValue = 0;
        if (m_inputNode->constData().absoluteMaximumIsProvided()) {
            scaledValue = absoluteValue;
        } else {
            double realMin = m_minValue * (m_negativeMinValue ? -1 : 1);
            double realMax = m_maxValue * (m_negativeMaxValue ? -1 : 1);
            scaledValue = realMin + (value * (realMax - realMin));
        }
		if (m_useInteger) {
			message.replace("<value>", QString::number(int(scaledValue)));
		} else {
            message.replace("<value>", QString::number(scaledValue, 'f', 4));
		}
        if (m_controller->getSendCustomOscToEos()) {
            m_controller->lightingConsole()->sendMessage(message);
        } else {
            m_controller->customOsc()->sendMessage(message);
        }
        emit messageSent();
    } else if (message.contains("<bpm>")) {
        if (absoluteValue <= 0) {
            absoluteValue = 0.1;
        }
        double scaledValue = 60.0 / absoluteValue;
        if (m_useInteger) {
            message.replace("<bpm>", QString::number(int(scaledValue)));
        } else {
            message.replace("<bpm>", QString::number(scaledValue, 'f', 4));
        }
        if (m_controller->getSendCustomOscToEos()) {
            m_controller->lightingConsole()->sendMessage(message);
        } else {
            m_controller->customOsc()->sendMessage(message);
        }
        emit messageSent();
    } else if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
        if (m_controller->getSendCustomOscToEos()) {
            m_controller->lightingConsole()->sendMessage(message);
        } else {
            m_controller->customOsc()->sendMessage(message);
        }
		emit messageSent();
	}
    m_lastValue = absoluteValue;
}

void OscOutBlock::setMinValue(double value) {
	if (value < 0) {
		value = 0;
	}
    m_minValue = value;
}

void OscOutBlock::setMaxValue(double value) {
    if (value < 0) {
        value = 0;
	}
    m_maxValue = value;
}
