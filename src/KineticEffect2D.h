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

#ifndef KINETICEFFECT2D_H
#define KINETICEFFECT2D_H

#include "utils.h"
#include "QCircularBuffer.h"

#include <QObject>
#include <QVector2D>

#include <limits>

namespace KineticEffect2DConstants {

/**
 * @brief DEFAULT_HISTORY_SIZE is the length of the history of events to store
 * @memberof KineticEffect2D
 */
static const int DEFAULT_HISTORY_SIZE = 5;
/**
 * @brief FPS is the update rate of this effect
 * @memberof KineticEffect2D
 */
static const int FPS = 50;

}


/**
 * @brief The MoveEvent2D struct describes a 2-dimensional move event
 */
struct MoveEvent2D {
	/**
	 * @brief position is the position of the move at that time
	 */
	QVector2D position;
	/**
	 * @brief time is the time when the event happened
	 */
	HighResTime::time_point_t time;
};


/**
 * @brief The KineticEffect2D class is used to generate a "kinetic" effect in two dimensions.
 *
 * "Kinetic" means to simulate the moving behavior of a real physial object (inertia and friction)
 * based in the speed and direction (velocity) of a touch movement.
 */
class KineticEffect2D : public QObject
{
	Q_OBJECT

	Q_PROPERTY(qreal friction MEMBER m_friction)

public:
	explicit KineticEffect2D(QObject* parent = 0);

signals:
	/**
	 * @brief velocityChanged is emitted when the velocity has changed
	 */
	void velocityChanged();
	/**
	 * @brief moving is emitted when the simulated object moves (manually or by inertia)
	 * @param position the current position ob the object
	 * @param origin of the move / touch drag
	 * @param isManual is true, if this movement is manual from an existing touch
	 */
	void moving(QVector2D position, QVector2D origin, bool isManual);

public slots:

	/**
	 * @brief start has to be called at the beginning of a new touch movement
	 * - it initializes all values for the new movement
	 * @param x position
	 * @param y position
	 */
	void start(qreal x, qreal y);

	/**
	 * @brief update has to be called when the position of the touch movement changes
	 * - it updates the position and velocity and adds the event to the history
	 * @param x position
	 * @param y position
	 */
	void update(qreal x, qreal y);

	/**
	 * @brief stop has to be called when the touch movement ends (the finger is lifted)
	 * - it calculates the velocity of the movement and starts the friction simulation
	 * @param x position
	 * @param y position
	 */
	void stop(qreal x, qreal y);

	/**
	 * @brief cancel can be called to cancel the movement, so that the velocity won't be changed
	 */
	void cancel();

	/**
	 * @brief setValue sets the value (position) state of this effect
	 * (i.e. when the position was changed without a touch movement)
	 * @param x new position
	 * @param y new position
	 */
	void setValue(qreal x, qreal y) { m_value = QVector2D(x, y); }

	/**
	 * @brief getVelocity returns the current velocity
	 * @return the velocity
	 */
	QVector2D getVelocity() const { return m_velocity; }


private slots:

	/**
	 * @brief applyDistance applies a distance to the position and checks if it is above minDistance
	 * @param distance to be applied
	 */
	void applyDistance(QVector2D distance);

	/**
	 * @brief updateVelocity calculates a new velocity value and emits the "moving" signal
	 */
	void updateVelocity();

	/**
	 * @brief triggerVelocityUpdate calls the updateVelocity() function the next frame
	 */
	void triggerVelocityUpdate();

private:
	/**
	 * @brief m_history stores the last MoveEvent2D events
	 */
	Qt3DCore::QCircularBuffer<MoveEvent2D> m_history;
	/**
	 * @brief m_isManual is true if the current movement is manual
	 */
	bool m_isManual;
	/**
	 * @brief m_origin is the origin of the touch movement
	 */
	QVector2D m_origin;
	/**
	 * @brief m_velocity is the current velocity of the movement
	 */
	QVector2D m_velocity;
	/**
	 * @brief m_value is the current position of the movement
	 */
	QVector2D m_value;
	/**
	 * @brief m_minDistance is the minimum distance per frame before the movement stops
	 */
	qreal m_minDistance;
	/**
	 * @brief m_minVelocity is the minimum velocity before the movement stops
	 */
	qreal m_minVelocity;
	/**
	 * @brief m_friction is the friction to be applied to the velocity per frame [~0.08...0.03]
	 */
	qreal m_friction;
	/**
	 * @brief m_lastUpdate is the time of the last movement update
	 */
	HighResTime::time_point_t m_lastUpdate;

};

#endif // KINETICEFFECT2D_H
