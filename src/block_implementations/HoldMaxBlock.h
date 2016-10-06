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

#ifndef HOLDMAXBLOCK_H
#define HOLDMAXBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include <QTimer>


class HoldMaxBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal holdTime READ getHoldTime WRITE setHoldTime NOTIFY holdTimeChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Hold Max";
        info.category << "Logic";
		info.helpText = "Outputs the highest value in the last x seconds.";
		info.qmlFile = "qrc:/qml/Blocks/HoldMaxBlock.qml";
		info.complete<HoldMaxBlock>();
		return info;
	}

	explicit HoldMaxBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void holdTimeChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getHoldTime() const { return m_holdTime; }
	void setHoldTime(qreal value) { m_holdTime = limit(0, value, 999); emit holdTimeChanged(); }

private slots:
	void onInputChanged();

	void onHoldTimeEnd();

protected:
	double		m_recentMaxValue;

	qreal		m_holdTime;

	QTimer		m_holdTimer;
};


#endif // HOLDMAXBLOCK_H
