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

#ifndef EOSKEYBLOCK_H
#define EOSKEYBLOCK_H

#include "block_data/OneInputBlock.h"


class EosKeyBlock : public OneInputBlock
{
	Q_OBJECT

	Q_PROPERTY(QString labelText READ getLabelText NOTIFY keyNameChanged)
	Q_PROPERTY(QString keyName READ getKeyName WRITE setKeyName NOTIFY keyNameChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Eos Key";
		info.nameInUi = "Key";
		info.category << "Eos";
		info.qmlFile = "qrc:/qml/Blocks/EosKeyBlock.qml";
		info.helpText = "Triggers the selected Key on the Eos.\n"
						"Can be resized by dragging the two vertical lines.";
		info.complete<EosKeyBlock>();
		return info;
	}

	explicit EosKeyBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void keyNameChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onInputChanged();

	void onButtonPress();
	void onButtonRelease();

	QStringList getLabelTexts() const;
	QStringList getKeyNames() const;

	QString getLabelText() const;

	QString getKeyName() const { return m_keyName; }
	void setKeyName(QString value);

private:
	void addKey(QString keyName, QString label);

protected:
	QString m_keyName;
	QStringList m_keyNames;
	QMap<QString, QString> m_keyLabels;
	double m_lastValue;
};

#endif // EOSKEYBLOCK_H
