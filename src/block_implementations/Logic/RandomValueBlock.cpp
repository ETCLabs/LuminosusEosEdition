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

#include "core/MainController.h"
#include "core/Nodes.h"

double getRandomValue() {
    return static_cast<float>(qrand()) / static_cast<float>(RAND_MAX);
}


RandomValueBlock::RandomValueBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_fadeRatio(this, "fadeRatio", 0.5)
    , m_stepTime(this, "stepTime", 1.0, 0.001, 99999)
    , m_timeVaration(this, "timeVariation", 0.5)
    , m_currentStepTime(m_stepTime)
	, m_pos(0.0)
{
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
	connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
	qsrand(static_cast<unsigned>(time(0)));
    updateRequestedSize();
}

void RandomValueBlock::onInputChanged() {
    if (!m_inputNode->constData().absoluteMaximumIsProvided()) return;
    double absoluteValue = m_inputNode->getAbsoluteValue();
    m_stepTime.setValue(absoluteValue);
}

void RandomValueBlock::sync() {
    m_pos = 1;
}

void RandomValueBlock::fillRandom(HsvMatrix& matrix) const {
    for (int x=0; x < matrix.width(); ++x) {
        for (int y=0; y < matrix.height(); ++y) {
            matrix.at(x, y).v = getRandomValue();
        }
    }
}

void RandomValueBlock::eachFrame(double timeSinceLastFrame) {
    // advance position value:
    double stepTime = m_currentStepTime;
    if (m_inputNode->isConnected() && !m_inputNode->constData().absoluteMaximumIsProvided()) {
        // there is a non-absolute Block connected:
        stepTime *= m_inputNode->getValue();
    }
    if (stepTime == 0) return;
    double progress = timeSinceLastFrame / stepTime;
	if ((m_pos + progress) > 1) {
		// get new values:
        m_oldValues = m_newValues;
        fillRandom(m_newValues);
        m_currentStepTime = m_stepTime + m_stepTime * m_timeVaration * (getRandomValue() * 2 - 1);
        if (m_currentStepTime <= 0) {
            m_currentStepTime = 0.1;
        }
	}
	m_pos = fmod(m_pos + progress, 1.0);

	// calculate current value:
    double fadePos = m_pos / m_fadeRatio;
    HsvDataModifier out(m_outputNode);
	if (fadePos < 1) {
        for (int x=0; x < out.width; ++x) {
            for (int y=0; y < out.height; ++y) {
                double newValue = m_oldValues.at(x, y).v * (1 - fadePos) + m_newValues.at(x, y).v * fadePos;
                out.set(x, y, 0, 0, newValue);
            }
        }
	} else {
        out.setFrom(m_newValues);
    }
}

void RandomValueBlock::updateRequestedSize() {
    Size s = m_outputNode->getRequestedSize();
    if (s == m_oldValues.size() && s == m_newValues.size()) return;

    m_oldValues.rescale(s);
    m_newValues.rescale(s);
    fillRandom(m_oldValues);
    fillRandom(m_newValues);
}
