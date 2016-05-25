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

#include "EosCueBlock.h"

#include "MainController.h"
#include "NodeBase.h"


EosCueBlock::EosCueBlock(MainController *controller, QString uid)
	: OneInputBlock(controller, uid, info().qmlFile)
	, m_cueList(1)
	, m_cueNumber(1)
	, m_lastValue(0.0)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosCueBlock::getState() const {
	QJsonObject state;
	state["cueList"] = getCueList();
	state["cueNumber"] = getCueNumber();
	return state;
}

void EosCueBlock::setState(const QJsonObject &state) {
	setCueList(state["cueList"].toInt());
	setCueNumber(state["cueNumber"].toDouble());
}

void EosCueBlock::onValueChanged() {
	double value = m_inputNode->data->getValue();
	if (value >= 0.5 && m_lastValue < 0.5) {
		QString message = "/eos/cue/%1/%2/fire";
		message = message.arg(QString::number(m_cueList), QString::number(m_cueNumber, 'g', 2));
		m_controller->osc()->sendMessage(message);
	}
	m_lastValue = value;
}

void EosCueBlock::setCueNumber(double value) {
	// round to two decimals:
	value = qRound(value * 100) / 100.0;
	m_cueNumber = limit(1, value, 9999); emit cueNumberChanged();
}
