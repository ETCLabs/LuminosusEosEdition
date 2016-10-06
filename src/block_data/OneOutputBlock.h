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

#ifndef ONEOUTPUTBLOCK_H
#define ONEOUTPUTBLOCK_H

#include "BlockBase.h"
#include <QPointer>

// forward declaration to reduce dependencies
class NodeBase;

/**
 * @brief The OneOutputBlock class is the base of all simple blocks with one output node.
 * It has a "value" property because most of these blocks generate a single value.
 */
class OneOutputBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)

public:

	/**
	 * @brief OneOutputBlock creates a Block instance with one output node
	 * @param controller a pointer to the main controller
	 * @param uid of this block or nothing if this block is new
	 * @param qmlUrl url to the QML file defining the UI component for this block
	 */
	explicit OneOutputBlock(MainController* controller, QString uid, QString qmlUrl);

	virtual QJsonObject getState() const override;

	virtual void setState(const QJsonObject& state) override;

signals:
	/**
	 * @brief valueChanged is emitted when the value changes
	 */
    void valueChanged();

public slots:
	/**
	 * @brief getValue returns the value (stored in the output node)
	 * @return the value [0...1]
	 */
	double getValue() const;
	/**
	 * @brief setValue sets the value (it is stored in the output node)
	 * @param value to set
	 */
	void setValue(double value);

protected:
	/**
	 * @brief m_outputNode is a pointer to the created output node
	 */
    QPointer<NodeBase> m_outputNode;

};

#endif // ONEOUTPUTBLOCK_H
