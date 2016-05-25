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

#ifndef DIMMERBLOCK_H
#define DIMMERBLOCK_H

#include "block_data/BlockBase.h"
#include "NodeBase.h"

class DimmerBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Dimmer";
		info.category << "Other";
		info.dependencies = {BlockDependency::NonETC};
		info.qmlFile = "qrc:/qml/Blocks/DimmerBlock.qml";
		info.complete<DimmerBlock>();
		return info;
	}

    int m_channel = 0;
    OutputNodeHsv* outputNode = nullptr;
    InputNodeHsv* inputNode = nullptr;
    static int instances;
    QString name = "";

	explicit DimmerBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;

	virtual void setState(const QJsonObject& state) override;

	double getValue() const;
    void setValue(double value) { inputNode->data->setValue(value); valueChanged(); }
	int getChannel() const { return m_channel; }
	void setChannel(int channel);

signals:
    void nameChanged();
    void channelChanged();
    void valueChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
    void onValueChanged();
	QString getName() const { return name; }
    void setName(QString value) { name = value; nameChanged(); }
    QString getGuiName() { return name.length() ? name : QString::number(m_channel); }
};

#endif // DIMMERBLOCK_H
