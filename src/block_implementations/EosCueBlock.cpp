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
#include "Nodes.h"


EosCueBlock::EosCueBlock(MainController *controller, QString uid)
    : InOutBlock(controller, uid, info().qmlFile)
    , m_cueNumber(1, 1, 0)
	, m_lastValue(0.0)
    , m_cueObject(nullptr)
    , m_isActive(false)
{
    connect(m_controller->eosManager(), SIGNAL(cueInfoChanged()), this, SLOT(onActiveCueChanged()));
    connect(this, SIGNAL(cueObjectChanged()), this, SLOT(onActiveCueChanged()));
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
    updateCueObject();
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
    double value = m_inputNode->getValue();
	if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
		QString message = "/eos/cue/%1/%2/fire";
        message = message.arg(QString::number(m_cueNumber.list), m_cueNumber.number);
        m_controller->eosConnection()->sendMessage(message);
	}
    m_lastValue = value;
}

void EosCueBlock::onActiveCueChanged() {
    if (!m_cueObject) {
        m_isActive = false;
        return;
    }
    if (m_cueObject->getIsActive()) {
        if (!m_isActive) {
            m_outputNode->setValue(1.0);
            QTimer::singleShot(500, this, SLOT(onImpulseEnd()));
        }
        m_isActive = true;
    } else {
        m_isActive = false;
    }
}

void EosCueBlock::onImpulseEnd() {
    m_outputNode->setValue(0.0);
}

void EosCueBlock::setCueList(int value) {
    if (value == m_cueNumber.list) return;
    m_cueNumber.list = limit(1, value, 999);
    emit cueListChanged();
    setCueObject(nullptr);
    updateCueObject();
}

void EosCueBlock::setCueNumber(double value) {
    // round to two decimals:
    value = qRound(value * 100) / 100.0;
    value = limit(1, value, 9999.99);
    if (value == m_cueNumber.numberAsInt / 100) return;
    m_cueNumber = EosCueNumber(m_cueNumber.list, value, 0);
    emit cueNumberChanged();
    setCueObject(nullptr);
    updateCueObject();
}

void EosCueBlock::setCueNumber(const EosCueNumber& value) {
    m_cueNumber = value;
    emit cueNumberChanged();
    emit cueListChanged();
    setCueObject(nullptr);
    updateCueObject();
}

void EosCueBlock::updateCueObject() {
    EosCueList* cueList = m_controller->cueListManager()->getCueList(m_cueNumber.list);
    if (cueList && cueList->isValid()) {
        EosCue* cue = cueList->getCue(m_cueNumber);
        if (cue && cue->isValid()) {
            setCueObject(cue);
            return;
        }
    }
    // could not find Cue -> retry in 5s:
    QTimer::singleShot(5000, this, SLOT(updateCueObject()));
}
