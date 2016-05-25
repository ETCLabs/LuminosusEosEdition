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

#include "KineticEffect.h"

#include <QTimer>
#include <QDebug>


KineticEffect::KineticEffect(QObject* parent)
	: QObject(parent)
	, m_history(KineticEffectConstants::DEFAULT_HISTORY_SIZE)
	, m_isManual(false)
	, m_origin(0.0)
	, m_velocity(0.0)
	, m_value(0.0)
	, m_minDistance(0.1)
	, m_minVelocity(0.5)
	, m_friction(0.05)
	, m_lastUpdate(HighResTime::now())
	, m_minValue(std::numeric_limits<qreal>::lowest())
	, m_maxValue(std::numeric_limits<qreal>::max())
{
}

void KineticEffect::start(qreal value) {
	m_isManual = true;
	m_velocity = 0.0;
	m_origin = value;
	MoveEvent moveEvent {value, HighResTime::now()};
	m_history.push_back(moveEvent);
}

void KineticEffect::update(qreal value) {
	qreal lastValue = m_history.last().value;
	qreal distance = value - lastValue;
	applyDistance(distance);
	MoveEvent moveEvent {value, HighResTime::now()};
	m_history.push_back(moveEvent);
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect::stop(qreal value) {
	m_isManual = false;
	qreal distance = value - m_history.last().value;
	applyDistance(distance);

	MoveEvent newestSample {value, HighResTime::now()};

	// find oldest sample in the last 10/60s:
	int index = 0;
	double shortTimeInSec = (10. / 60.);
	MoveEvent oldestSampleInShortTime = m_history[index];
	while (HighResTime::diff(newestSample.time, oldestSampleInShortTime.time) > shortTimeInSec) {
		if (index >= m_history.size() - 1) {
			// reached end of history
			break;
		}
		++index;
		oldestSampleInShortTime = m_history[index];
	}
	qreal totalDistance = newestSample.value - oldestSampleInShortTime.value;
	qreal durationInSec = HighResTime::diff(newestSample.time, oldestSampleInShortTime.time);
	m_velocity = totalDistance / qMax(durationInSec, 0.0001);
	m_lastUpdate = newestSample.time;
	triggerVelocityUpdate();
}

void KineticEffect::cancel() {
	m_isManual = false;
	m_lastUpdate = m_history.last().time;
	triggerVelocityUpdate();
}

void KineticEffect::setValue(qreal value) {
	m_value = value;
}

void KineticEffect::applyDistance(qreal distance) {
	if (qAbs(distance) < m_minDistance) {
		m_velocity = 0.0;
	}
	m_value += distance;
	// check bounds:
	if (m_value < m_minValue) {
		m_value = m_minValue;
		m_velocity = 0.0;
	} else if (m_value > m_maxValue) {
		m_value = m_maxValue;
		m_velocity = 0.0;
	}
}

void KineticEffect::updateVelocity() {
	if (qAbs(m_velocity) <= m_minVelocity) {
		m_velocity = 0.0;
		return;
	}
	m_velocity -= m_velocity * m_friction;
	qreal timeSinceLastUpdateInSec = HighResTime::getElapsedSecAndUpdate(m_lastUpdate);
	applyDistance(m_velocity * timeSinceLastUpdateInSec);
	triggerVelocityUpdate();
	velocityChanged();
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect::triggerVelocityUpdate() {
	QTimer::singleShot(1000 / KineticEffectConstants::FPS, this, SLOT(updateVelocity()));
}
