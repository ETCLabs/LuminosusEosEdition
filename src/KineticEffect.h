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

#ifndef KINETICEFFECT_H
#define KINETICEFFECT_H

#include "utils.h"

#include "QCircularBuffer.h"
#include <QAbstractAnimation>
#include <limits>

/**
 * @brief The KineticEffectConstants namespace contains all constants used in KineticEffect.
 */
namespace KineticEffectConstants {
	/**
	 * @brief DEFAULT_HISTORY_SIZE is the length of the history of events to store
	 */
    static const int DEFAULT_HISTORY_SIZE = 5;
}

/**
 * @brief The MoveEvent struct describes a move event in one dimension.
 */
struct MoveEvent {
	/**
	 * @brief value the value / position of the event at that time
	 */
	qreal value;
	/**
	 * @brief time the time when the event happened
	 */
	HighResTime::time_point_t time;
};


/**
 * @brief The KineticEffect class is used to generate a "kinetic" effect in one dimension.
 *
 * "Kinetic" means to simulate the moving behavior of a real physial object (inertia and friction)
 * based in the speed and direction (velocity) of a touch movement.
 */
class KineticEffect : public QAbstractAnimation
{
	Q_OBJECT

    Q_PROPERTY(qreal friction MEMBER m_friction)
    Q_PROPERTY(qreal minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(qreal maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)

public:
    explicit KineticEffect(QAbstractAnimation* parent = 0);

signals:
	/**
	 * @brief velocityChanged is emitted when the velocity has changed
	 */
	void velocityChanged();

	/**
	 * @brief moving is emitted when the simulated object moves (manually or by inertia)
	 * @param position the current position / value
	 * @param origin of the move / touch drag
	 * @param isManual is true, if this movement is manual from an existing touch
	 */
	void moving(qreal position, qreal origin, bool isManual);
	/**
	 * @brief minValueChanged is emitted when minValue changed
	 */
    void minValueChanged();
	/**
	 * @brief maxValueChanged is emitted when maxValue changed
	 */
    void maxValueChanged();

public slots:
	/**
	 * @brief start has to be called at the beginning of a new touch movement
	 * - it initializes all values for the new movement
	 * @param value at start
	 */
	void start(qreal value);
	/**
	 * @brief updatee has to be called when the position of the touch movement changes
	 * - it updates the position and velocity and adds the event to the history
	 * @param value current value / position of movement
	 */
	void update(qreal value);
	/**
	 * @brief stop has to be called when the touch movement ends (the finger is lifted)
	 * - it calculates the velocity of the movement and starts the friction simulation
	 * @param value at the end
	 */
	void stop(qreal value);

	/**
	 * @brief cancel can be called to cancel the movement, so that the velocity won't be changed
	 */
	void cancel();

	/**
	 * @brief setValue sets the value (position) state of this effect
	 * (i.e. when the position was changed without a touch movement)
	 * @param value new value / position
	 */
	void setValue(qreal value);

	/**
	 * @brief getVelocity returns the current velocity
	 * @return current velocity
	 */
	qreal getVelocity() const { return m_velocity; }

	/**
	 * @brief getMinValue returns the minimum value
	 * @return minValue
	 */
    qreal getMinValue() const { return m_minValue; }
	/**
	 * @brief setMinValue sets the minimum value that can be reached
	 * @param value new minValue
	 */
    void setMinValue(qreal value) { m_minValue = value; emit minValueChanged(); }

	/**
	 * @brief getMaxValue returns the maximum value to be reached
	 * @return maxValue
	 */
    qreal getMaxValue() const { return m_maxValue; }
	/**
	 * @brief setMaxValue sets the maximum value that can be reached
	 * @param value new maxValue
	 */
    void setMaxValue(qreal value) { m_maxValue = value; emit maxValueChanged(); }

    /**
     * @brief duration return the "duration" of this "animation",
     * -1 means it runs till it is stopped (when velocity < min_velocity)
     * @return
     */
    int duration() const override { return -1; }

protected:

    /**
     * @brief updateCurrentTime is called to progress the animation
     * (between start() and stop() calls)
     */
    void updateCurrentTime(int /*currentTime*/) override;

private slots:
	/**
	 * @brief applyDistance applies a distance to the position and checks if it is above minDistance
	 * @param distance to be applied
	 */
	void applyDistance(qreal distance);

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
	 * @brief m_history stores the last MoveEvent events
	 */
	Qt3DCore::QCircularBuffer<MoveEvent> m_history;
	/**
	 * @brief m_isManual is true if the current movement is manual
	 */
	bool m_isManual;
	/**
	 * @brief m_origin is the origin of the touch movement
	 */
	qreal m_origin;
	/**
	 * @brief m_velocity is the current velocity of the movement
	 */
	qreal m_velocity;
	/**
	 * @brief m_value is the current position of the movement
	 */
    qreal m_value;
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
	/**
	 * @brief m_minValue is the minimum value that can be reached (i.e. object borders)
	 */
	qreal m_minValue;
	/**
	 * @brief m_maxValue is the maximum value that can be reached (i.e. object borders)
	 */
    qreal m_maxValue;

};

#endif // KINETICEFFECT_H
