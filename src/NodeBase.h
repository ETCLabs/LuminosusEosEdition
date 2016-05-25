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

#ifndef NODEBASE_H
#define NODEBASE_H

#include "Matrix.h"

#include <QObject>
#include <QQuickItem>
#include <memory>

// Forward declaration to reduce dependencies
class BlockInterface;


/**
 * @brief The NodeBase class is the base of all Nodes.
 * It is responsible for the connection handling, active status and matrix size.
 */
class NodeBase : public QObject
{
    Q_OBJECT

public:
	explicit NodeBase(bool isOutput, int dataType, BlockInterface* block, int id);

    void setGuiItem(QQuickItem* item);
	QString getUid() const;

    void setActive(bool value);

signals:
    void dataChanged();
    void connectionChanged();
    void matrixSizeChanged();
    void activeChanged();

public slots:
    virtual void onTouched();
    virtual void connectTo(NodeBase* node);
    virtual void disconnect(bool isReconnect=false);
    virtual void focus();
    virtual void defocus();
    virtual void updateBezierCurve();

protected:
	virtual void copyDataBeforeDisconnect() {}
	virtual void setDataBeforeConnect() {}

protected:
	int m_id;
	int m_dataType;
public:  // TODO: should be protected
	const bool m_isOutput;
	bool m_connected;
	NodeBase* m_connectedNode;
public:  // TODO: should be protected
	bool m_active;
	std::vector<NodeBase*> m_dependOnActive;
	BlockInterface* m_block;
	QQuickItem* m_guiItem;

public:  // TODO: should be protected
	static bool m_focusExists;
	static NodeBase* m_focusedNode;
};

// --------------------------------------------

struct RequestedData {
    size_t sx = 1;
    size_t sy = 1;
};

// --------------------------------------------

class InputNodeHsv : public NodeBase
{
    Q_OBJECT

public:

	explicit InputNodeHsv(BlockInterface* block, int id);

    ~InputNodeHsv() {
		delete m_requestedData;
    }

	void setMatrixSize(size_t sx, size_t sy);

public:  // TODO: should be protected
	ColorMatrix* data = nullptr;
	RequestedData* m_requestedData = nullptr;
	bool m_active = true;
};

// --------------------------------------------

class OutputNodeHsv : public NodeBase
{
    Q_OBJECT

public:

	explicit OutputNodeHsv(BlockInterface* block, int id);

    ~OutputNodeHsv() {
		delete m_data;
    }

    void setDataBeforeConnect() override;
    void copyDataBeforeDisconnect() override;
	void setMatrixSize(size_t sx, size_t sy);

public slots:
    void setHsv(double h, double s, double v) {
		m_data->setHsv(h, s, v);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setHsv(const colorData2d& newHsv) {
		m_data->setHsv(newHsv);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setHsvAt(size_t x, size_t y, double h, double s, double v) {
		m_data->setHsvAt(x, y, h, s, v);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setRgb(double r, double g, double b) {
		m_data->setRgb(r, g, b);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setRgb(const colorData2d& newRgb) {
		m_data->setRgb(newRgb);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setRgbAt(size_t x, size_t y, double r, double g, double b) {
		m_data->setRgbAt(x, y, r, g, b);
		if (m_connected) m_connectedNode->dataChanged();
    }
    void setValue(double v) {
		if (v == m_data->getValue()) return;
		m_data->setValue(v);
		if (m_connected) m_connectedNode->dataChanged();
    }

public:  // TODO: should be protected
	ColorMatrix* m_data = nullptr;
	RequestedData* m_requestedData = nullptr;
	bool m_active;
	std::vector<InputNodeHsv*> m_dependOnMatrixSize;

};

#endif // NODEBASE_H
