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

#ifndef NOTESBLOCK_H
#define NOTESBLOCK_H

#include "core/block_data/BlockBase.h"


class NotesBlock : public BlockBase
{
	Q_OBJECT

	Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Notes";
		info.nameInUi = "Notes";
		info.category << "General" << "Other";
        info.helpText = "Can be used to take notes or to create informative labels.";
        info.qmlFile = "qrc:/qml/Blocks/Info/NotesBlock.qml";
		info.complete<NotesBlock>();
		return info;
	}

    explicit NotesBlock(MainController* controller, QString uid);

	virtual void getAdditionalState(QJsonObject& state) const override;
	virtual void setAdditionalState(const QJsonObject& state) override;

signals:
	void textChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	QString getText() const { return m_text; }
	void setText(const QString& value) { m_text = value; emit textChanged(); }

protected:
	QString m_text;
};

#endif // NOTESBLOCK_H
