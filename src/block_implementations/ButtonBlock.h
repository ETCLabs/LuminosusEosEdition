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

#ifndef BUTTONBLOCK
#define BUTTONBLOCK

#include "block_data/OneOutputBlock.h"

class ButtonBlock : public OneOutputBlock
{
	Q_OBJECT

	Q_PROPERTY(bool toggleMode READ getToggleMode WRITE setToggleMode NOTIFY toggleModeChanged)
	Q_PROPERTY(QString labelText READ getLabelText WRITE setLabelText NOTIFY labelTextChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Button";
        info.category << "Controls";
        info.helpText = "A simple button.\n"
                        "Outputs 1 if pressed and 0 if not.";
		info.qmlFile = "qrc:/qml/Blocks/ButtonBlock.qml";
		info.complete<ButtonBlock>();
		return info;
	}

	explicit ButtonBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void toggleModeChanged();
	void labelTextChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	bool getToggleMode() const { return m_toggleMode; }
	void setToggleMode(bool value) { m_toggleMode = value; emit toggleModeChanged(); }

	QString getLabelText() const { return m_labelText; }
	void setLabelText(QString value) { m_labelText = value; emit labelTextChanged(); }

protected:
	bool m_toggleMode;
	QString m_labelText;
};

#endif // BUTTONBLOCK

