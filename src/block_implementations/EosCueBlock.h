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

#ifndef EOSCUEBLOCK_H
#define EOSCUEBLOCK_H

#include "block_data/InOutBlock.h"
#include "eos_specific/EosCue.h"
#include "utils.h"


class EosCueBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(int cueList READ getCueList WRITE setCueList NOTIFY cueListChanged)
	Q_PROPERTY(double cueNumber READ getCueNumber WRITE setCueNumber NOTIFY cueNumberChanged)
    Q_PROPERTY(EosCue* cueObject READ getCueObject WRITE setCueObject NOTIFY cueObjectChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Eos Cue";
		info.nameInUi = "Cue";
		info.category << "Eos";
        info.helpText = "Fires the selected Cue when the input value is greater than 0.";
		info.qmlFile = "qrc:/qml/Blocks/EosCueBlock.qml";
		info.complete<EosCueBlock>();
		return info;
	}

	explicit EosCueBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void cueListChanged();
	void cueNumberChanged();
    void cueObjectChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    void onActiveCueChanged();

    void onImpulseEnd();

    int getCueList() const { return m_cueNumber.list; }
    void setCueList(int value);

    double getCueNumber() const { return m_cueNumber.numberAsInt / 100.0; }
	void setCueNumber(double value);

    EosCue* getCueObject() const { return m_cueObject; }
    void setCueObject(EosCue* value) { m_cueObject = value; emit cueObjectChanged(); }

    void setCueNumber(const EosCueNumber& value);

private slots:
    void updateCueObject();

protected:
    EosCueNumber m_cueNumber;
    double m_lastValue;
    QPointer<EosCue> m_cueObject;
    bool m_isActive;
};

#endif // EOSCUEBLOCK_H
