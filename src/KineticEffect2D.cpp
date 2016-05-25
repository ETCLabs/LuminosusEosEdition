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

#include "KineticEffect2D.h"

#include <QTimer>
#include <QDebug>

KineticEffect2D::KineticEffect2D(QObject* parent)
	: QObject(parent)
	, m_history(KineticEffect2DConstants::DEFAULT_HISTORY_SIZE)
	, m_isManual(false)
	, m_origin(0, 0)
	, m_velocity(0, 0)
	, m_value(0, 0)
	, m_minDistance(0.1)
	, m_minVelocity(0.5)
	, m_friction(0.05)
	, m_lastUpdate(HighResTime::now())
{
}

void KineticEffect2D::start(qreal x, qreal y) {
	QVector2D pos(x, y);
	m_isManual = true;
	m_velocity = QVector2D(0, 0);
	m_origin = pos;
	MoveEvent2D moveEvent {pos, HighResTime::now()};
	m_history.push_back(moveEvent);
}

void KineticEffect2D::update(qreal x, qreal y) {
	QVector2D pos(x, y);
	QVector2D lastPos = m_history.last().position;
	QVector2D distance = pos - lastPos;
	applyDistance(distance);
	MoveEvent2D moveEvent {pos, HighResTime::now()};
	m_history.push_back(moveEvent);
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect2D::stop(qreal x, qreal y) {
	QVector2D pos(x, y);
	m_isManual = false;
	QVector2D lastPos = m_history.last().position;
	QVector2D distance = pos - lastPos;
	applyDistance(distance);

	MoveEvent2D newestSample {pos, HighResTime::now()};

	// find oldest sample in the last 10/60s:
	int index = 0;
	double shortTimeInSec = 10. / 60.;
	MoveEvent2D oldestSampleInShortTime = m_history[index];
	while (HighResTime::diff(newestSample.time, oldestSampleInShortTime.time) > shortTimeInSec) {
		if (index >= m_history.size() - 1) {
			// reached end of history
			break;
		}
		++index;
		oldestSampleInShortTime = m_history[index];
	}
	QVector2D totalDistance = newestSample.position - oldestSampleInShortTime.position;
	qreal durationInSec = HighResTime::diff(newestSample.time, oldestSampleInShortTime.time);
	m_velocity = totalDistance / qMax(durationInSec, 0.0001);
	m_lastUpdate = newestSample.time;
	triggerVelocityUpdate();
}

void KineticEffect2D::cancel() {
	m_isManual = false;
	m_lastUpdate = m_history.last().time;
	triggerVelocityUpdate();
}

void KineticEffect2D::applyDistance(QVector2D distance) {
	m_value += distance;
	if (distance.length() < m_minDistance) {
		m_velocity = QVector2D(0, 0);
        m_value = QVector2D(qRound(m_value.x()), qRound(m_value.y()));
	}
}

void KineticEffect2D::updateVelocity() {
	if (m_velocity.length() <= m_minVelocity) {
		m_velocity = QVector2D(0, 0);
        m_value = QVector2D(qRound(m_value.x()), qRound(m_value.y()));
		emit moving(m_value, m_origin, m_isManual);
		return;
	}
	m_velocity -= m_velocity * m_friction;
	qreal timeSinceLastUpdateInSec = HighResTime::getElapsedSecAndUpdate(m_lastUpdate);
	applyDistance(m_velocity * timeSinceLastUpdateInSec);
	triggerVelocityUpdate();
	velocityChanged();
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect2D::triggerVelocityUpdate() {
	QTimer::singleShot(1000 / KineticEffect2DConstants::FPS, this, SLOT(updateVelocity()));
}
