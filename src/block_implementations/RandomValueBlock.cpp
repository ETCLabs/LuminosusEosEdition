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

#include "RandomValueBlock.h"

#include "MainController.h"
#include "Nodes.h"


RandomValueBlock::RandomValueBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid, info().qmlFile)
	, m_fadeRatio(0.5)
	, m_stepTime(1.0)
    , m_timeVaration(0.5)
	, m_oldValue(0)
	, m_newValue(0)
    , m_currentStepTime(m_stepTime)
	, m_pos(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
	connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
	qsrand(static_cast<unsigned>(time(0)));
	m_oldValue = getRandomValue();
    m_newValue = getRandomValue();
}

QJsonObject RandomValueBlock::getState() const {
    QJsonObject state;
    state["fadeRatio"] = getFadeRatio();
    state["stepTime"] = getStepTime();
    state["timeVariation"] = getTimeVariation();
    return state;
}

void RandomValueBlock::setState(const QJsonObject& state) {
    setFadeRatio(state["fadeRatio"].toDouble());
    setStepTime(state["stepTime"].toDouble());
    setTimeVariation(state["timeVariation"].toDouble());
}

void RandomValueBlock::onInputChanged() {
    if (!m_inputNode->getDataToRead().absoluteMaximumIsProvided()) return;
    double absoluteValue = m_inputNode->getAbsoluteValue();
    setStepTime(absoluteValue);
}

void RandomValueBlock::sync() {
    m_pos = 1;
}

double RandomValueBlock::getRandomValue() {
    return static_cast<float>(qrand()) / static_cast<float>(RAND_MAX);
}

void RandomValueBlock::eachFrame(double timeSinceLastFrame) {
    // advance position value:
    double stepTime = m_currentStepTime;
    if (m_inputNode->isConnected() && !m_inputNode->getDataToRead().absoluteMaximumIsProvided()) {
        // there is a non-absolute Block connected:
        stepTime *= m_inputNode->getValue();
    }
    if (stepTime == 0) return;
    double progress = timeSinceLastFrame / stepTime;
	if ((m_pos + progress) > 1) {
		// get new values:
		m_oldValue = m_newValue;
		m_newValue = getRandomValue();
        m_currentStepTime = m_stepTime + m_stepTime * m_timeVaration * (getRandomValue() * 2 - 1);
        if (m_currentStepTime <= 0) {
            m_currentStepTime = 0.1;
        }
	}
	m_pos = fmod(m_pos + progress, 1.0);

	// calculate current value:
	double fadePos = m_pos / m_fadeRatio;
	double currentValue = 0.0;
	if (fadePos < 1) {
		currentValue = m_oldValue * (1 - fadePos) + m_newValue * fadePos;
	} else {
		currentValue = m_newValue;
	}
	m_outputNode->setValue(currentValue);
}
