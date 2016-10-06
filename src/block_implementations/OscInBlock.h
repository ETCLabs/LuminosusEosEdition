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

#ifndef OSCINBLOCK_H
#define OSCINBLOCK_H

#include "block_data/OneOutputBlock.h"
#include "OSCMessage.h"


class OscInBlock : public OneOutputBlock
{
	Q_OBJECT

	Q_PROPERTY(QString message READ getMessage WRITE setMessage NOTIFY messageChanged)
	Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
	Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "OSC In";
		info.nameInUi = "OSC Input";
        info.category << "Custom OSC";
		info.helpText = "Enter the path of an OSC message to listen for.\n\n"
						"Outputs the value of the first parameter of any matching incoming message.\n"
						"The expected value range of the parameter can be specified above.\n\n"
						"If the incoming message has no parameters a short pulse of on and off will be emitted "
						"for each message received.";
		info.qmlFile = "qrc:/qml/Blocks/OscInBlock.qml";
		info.complete<OscInBlock>();
		return info;
	}

	explicit OscInBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void messageChanged();
	void minValueChanged();
	void maxValueChanged();
	void validMessageReceived();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onMessageReceived(OSCMessage msg);

	void onEndOfPulse();

    void updateConnection();

	QString getMessage() const { return m_message; }
	void setMessage(const QString& value) { m_message = value; emit messageChanged(); }

	double getMinValue() const { return m_minValue; }
	void setMinValue(double value);

	double getMaxValue() const { return m_maxValue; }
	void setMaxValue(double value);

protected:
	QString m_message;
	double m_minValue;
	double m_maxValue;
};

#endif // OSCINBLOCK_H
