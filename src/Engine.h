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

#ifndef ENGINE_H
#define ENGINE_H

#include "utils.h"

#include <QObject>
#include <QTimer>
#include <chrono>


/**
 * @brief The Engine class defines an engine that is responsible to trigger all non-GUI actions
 * that have to be done regulary (i.e. block logic and data output).
 * It generates different signals with a specified FPS rate.
 *
 * It is independent from the GUI. Actions that affect the GUI should be triggered by QTimer.
 */
class Engine : public QObject
{
	Q_OBJECT

public:
	/**
	 * @brief Engine creates an engine instance
	 * @param fps the amount of frames per second to generate
	 * @param parent QObject parent
	 */
	explicit Engine(int fps = 50, QObject *parent = 0);

signals:
	/**
	 * @brief updateBlocks is emitted every frame when the block logic should update its values
	 * @param timeSinceLastFrame is the time in seconds since the last call of this signal
	 */
    void updateBlocks(double timeSinceLastFrame);
	/**
	 * @brief updateOutput is emitted every frame after the blocks have been updated
	 * and the output (i.e. ArtNet, sACN) should be updated
	 * @param timeSinceLastFrame is the time in seconds since the last call of this signal
	 */
    void updateOutput(double timeSinceLastFrame);

public slots:

	/**
	 * @brief start starts the engine, signals will then be emitted every frame
	 */
    void start() {
		m_timer.start(1000 / m_fps);
    }

	/**
	 * @brief stop stops the engine, no more signals will be emitted
	 */
    void stop() {
		m_timer.stop();
    }

private slots:

	/**
	 * @brief tick is the internal function called every frame
	 * and emits the signals in the correct order
	 */
	void tick();

private:
	/**
	 * @brief m_timer is the timer that triggers the tick() function
	 */
	QTimer m_timer;
	/**
	 * @brief m_fps is the amount of frames per second to be generated
	 */
	int m_fps;
	/**
	 * @brief m_lastFrameTime is the time of the last generated frame
	 */
	HighResTime::time_point_t m_lastFrameTime;

};

#endif // ENGINE_H
