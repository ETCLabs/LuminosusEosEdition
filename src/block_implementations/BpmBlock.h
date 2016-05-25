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

#ifndef BPMBLOCK_H
#define BPMBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include "QCircularBuffer.h"
#include <QElapsedTimer>

namespace BpmConstants {

static const int HISTORY_LENGTH = 5;

static const int MIN_BPM = 60;
static const int MAX_BPM = 160;

}


class BpmBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal bpm READ getBpm WRITE setBpm NOTIFY bpmChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "BPM";
		info.category = QStringList {"Logic"};
		info.dependencies = {BlockDependency::Experimental};
		info.qmlFile = "qrc:/qml/Blocks/BpmBlock.qml";
		info.complete<BpmBlock>();
		return info;
	}

	explicit BpmBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void bpmChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getBpm() const { return m_bpm; }
	void setBpm(qreal value) { m_bpm = value; emit bpmChanged(); }

private:
	void updateBpm();

private slots:
	void onInputChanged();

protected:
	/**
	 * @brief m_bpm current detected BPM value
	 */
	qreal			m_bpm;
	/**
	 * @brief m_timer QElapsedTimer to get the time elapsed since the last beat
	 */
	QElapsedTimer	m_timer;
	/**
	 * @brief m_lastBeats stores the times of the last beats in seconds since start
	 */
	Qt3DCore::QCircularBuffer<qint64> m_lastBeats;
	/**
	 * @brief m_lastValue stores the last input value
	 */
	double			m_lastValue;
};

#endif // BPMBLOCK_H
