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

#include "core/block_data/OneInputBlock.h"


class EosKeyBlock : public OneInputBlock
{
	Q_OBJECT

	Q_PROPERTY(QString labelText READ getLabelText NOTIFY keyNameChanged)
	Q_PROPERTY(QString keyName READ getKeyName WRITE setKeyName NOTIFY keyNameChanged)
    Q_PROPERTY(QString customLabel READ getCustomLabel WRITE setCustomLabel NOTIFY customLabelChanged)
    Q_PROPERTY(QString customKeyName READ getCustomKeyName WRITE setCustomKeyName NOTIFY customKeyNameChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Eos Key";
		info.nameInUi = "Key";
		info.category << "ETC Consoles" << "Eos";
        info.helpText = "Triggers the selected Key on the Eos.<br>"
                        "Can be resized by dragging the two vertical lines.<br><br>"
                        "The OSC key names can be found here: <a href=\"https://github.com/ElectronicTheatreControlsLabs/OSCLayouts/blob/master/Eos%20OSC%20Keys.pdf\">Eos OSC Keys</a><br><br>"
                        "(https://github.com/ElectronicTheatreControlsLabs/OSCLayouts/blob/master/Eos%20OSC%20Keys.pdf)\n\n"
                        "It uses Eos OSC user 0 if 'run in background' is enabled, otherwise user 1.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosKeyBlock.qml";
		info.complete<EosKeyBlock>();
		return info;
	}

	explicit EosKeyBlock(MainController* controller, QString uid);

	virtual void getAdditionalState(QJsonObject& state) const override;
	virtual void setAdditionalState(const QJsonObject& state) override;

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

signals:
	void keyNameChanged();
    void customLabelChanged();
    void customKeyNameChanged();

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

    QString getCustomLabel() const { return m_customLabel; }
    void setCustomLabel(QString value);

    QString getCustomKeyName() const { return m_customKeyName; }
    void setCustomKeyName(QString value) { m_customKeyName = value; emit customKeyNameChanged(); }

private:
    void addAllKeys();
	void addKey(QString keyName, QString label);

protected:
	QString m_keyName;
	QStringList m_keyNames;
	QMap<QString, QString> m_keyLabels;
	double m_lastValue;
    QString m_customLabel;
    QString m_customKeyName;
    BoolAttribute m_runInbackground;
};

#endif // EOSKEYBLOCK_H
