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

#include "BpmBlock.h"

#include "Nodes.h"
#include "MainController.h"  // for LuminosusConstants

BpmBlock::BpmBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid, info().qmlFile)
	, m_bpm(120)
	, m_lastBeats(BpmConstants::HISTORY_LENGTH)
	, m_lastValue(0)
    , m_factor(1)
{
    m_startTime = HighResTime::now();
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    setBpm(120);
}

QJsonObject BpmBlock::getState() const {
	QJsonObject state;
	state["bpm"] = getBpm();
    state["factor"] = getFactor();
	return state;
}

void BpmBlock::setState(const QJsonObject &state) {
    setBpm(state["bpm"].toDouble());
    setFactor(state["factor"].toDouble());
}

void BpmBlock::triggerBeat() {
    double beatTime = HighResTime::elapsedSecSince(m_startTime);
    // check if history should be discarded:
    if (!m_lastBeats.isEmpty()) {
        double secSinceLast = beatTime - m_lastBeats.last();
        if (secSinceLast > 60. / BpmConstants::MIN_BPM) {
            // yes last beat is too old -> discard history:
            m_lastBeats.clear();
        }
    }
    m_lastBeats.append(beatTime);
    updateBpm();
}

void BpmBlock::updateOutput() {
    double realBpm = m_bpm * m_factor;
    if (realBpm > 0) {
        m_outputNode->setAbsoluteValue(60.0 / realBpm);
    }
}

void BpmBlock::setFactor(double value) {
    m_factor = limit(0.1, value, 10);
    emit factorChanged();
    updateOutput();
}

void BpmBlock::setBpm(qreal value) {
    m_bpm = value;
    emit bpmChanged();
    updateOutput();
}

void BpmBlock::updateBpm() {
    // check if there are at least 2 values:
    if (m_lastBeats.size() < 2) return;

    double sumBeatDuration = 0;  // in sec
	for (int i=1; i<m_lastBeats.size(); ++i) {
		sumBeatDuration += m_lastBeats[i] - m_lastBeats[i-1];
	}
    // averageBeatDuration is in sec
    double averageBeatDuration = sumBeatDuration / (m_lastBeats.size() - 1);
    setBpm((1 / averageBeatDuration) * 60);
}

void BpmBlock::onInputChanged() {
    double value = m_inputNode->getValue();
	if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
		// this is a "beat"
        triggerBeat();
	}
	m_lastValue = value;
}
