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

#include "EosChanBlock.h"

#include "MainController.h"
#include "NodeBase.h"

EosChanBlock::EosChanBlock(MainController *controller, QString uid)
	: OneInputBlock(controller, uid, info().qmlFile)
	, m_chanNumber(1)
	, m_lastValue(0.0)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosChanBlock::getState() const {
	QJsonObject state;
	state["chanNumber"] = getChannelNumber();
	return state;
}

void EosChanBlock::setState(const QJsonObject &state) {
	setChannelNumber(state["chanNumber"].toInt());
}

void EosChanBlock::onValueChanged() {
	double value = m_inputNode->data->getValue();
	if (value == m_lastValue) return;
	QString message = "/eos/user/0/chan/%1=%2";
	message = message.arg(QString::number(m_chanNumber), QString::number(value * 100, 'g', 4));
	m_controller->osc()->sendMessage(message);
	m_lastValue = value;
}
