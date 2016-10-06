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

#ifndef COLORWHEELBLOCK_H
#define COLORWHEELBLOCK_H

#include "block_data/InOutBlock.h"

class ColorwheelBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(double hue READ getHue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(double sat READ getSat WRITE setSat NOTIFY satChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Colorwheel";
        info.category << "Controls";
        info.helpText = "Can be used in connection with Eos Channel or Group Block.\n\n"
                        "The intensity can be controlled with the input node.";
		info.qmlFile = "qrc:/qml/Blocks/ColorwheelBlock.qml";
		info.complete<ColorwheelBlock>();
		return info;
	}

    explicit ColorwheelBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void hueChanged();
    void satChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    double getHue() const { return m_hue; }
    void setHue(double value);

    double getSat() const { return m_sat; }
    void setSat(double value);

protected:
    double m_hue;
    double m_sat;
};

#endif // COLORWHEELBLOCK_H
