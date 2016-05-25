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

#include "NodeBase.h"

#include "block_data/BlockInterface.h"

#include <QTimer>
#include <QVector2D>

// initialize static member attributes:
bool NodeBase::m_focusExists = false;
NodeBase* NodeBase::m_focusedNode = nullptr;

NodeBase::NodeBase(bool isOutput, int dataType, BlockInterface* block, int id)
	: QObject(block)
	, m_id(id)
	, m_dataType(dataType)
	, m_isOutput(isOutput)
	, m_connected(false)
	, m_connectedNode(nullptr)
	, m_active(false)
	, m_dependOnActive()
	, m_block(block)
	, m_guiItem(nullptr)
{
    connect(block, SIGNAL(positionChanged()), this, SLOT(updateBezierCurve()));
    // update line when all layouts are correctly initialized:
	QTimer::singleShot(200, this, SLOT(updateBezierCurve()));
}

void NodeBase::setGuiItem(QQuickItem *item) {
    m_guiItem = item;
    connect(item, SIGNAL(touched()), this, SLOT(onTouched()));
}

QString NodeBase::getUid() const {
	return m_block->getUid().append("|").append(QString::number(m_id));
}

void NodeBase::onTouched()
{
	if (!m_focusExists) {
		m_guiItem->setProperty("focused", true);
        m_focusExists = true;
        m_focusedNode = this;
    } else if (m_focusedNode == this) {
        disconnect();
        m_guiItem->setProperty("focused", false);
        m_focusExists = false;
        m_focusedNode = nullptr;
    } else {
        auto otherNode = m_focusedNode;
        if (otherNode->m_guiItem) otherNode->m_guiItem->setProperty("focused", false);
        m_focusExists = false;
        m_focusedNode = nullptr;
        connectTo(otherNode);
    }
}

void showBezierCurve(QQuickItem* guiItem, QQuickItem* leftItem, QQuickItem* rightItem) {
    QQuickItem* bezierCurve = (QQuickItem*)(guiItem->findChild<QObject*>("bezierCurve"));
    bezierCurve->setProperty("leftItem", QVariant::fromValue<QQuickItem*>(leftItem));
    bezierCurve->setProperty("rightItem", QVariant::fromValue<QQuickItem*>(rightItem));
    bezierCurve->setProperty("visible", true);
}

void hideBezierCurve(QQuickItem* guiItem) {
    QQuickItem* bezierCurve = (QQuickItem*)(guiItem->findChild<QObject*>("bezierCurve"));
    bezierCurve->setProperty("leftItem", QVariant::fromValue<QQuickItem*>(guiItem));
    bezierCurve->setProperty("rightItem", QVariant::fromValue<QQuickItem*>(guiItem));
    bezierCurve->setProperty("visible", false);
}

void NodeBase::connectTo(NodeBase* node)
{
    if (node->m_dataType != this->m_dataType) return;  // nodes aren't of the same type
    if (node->m_block == this->m_block) return;  // nodes are on same block
    if (node->m_isOutput == this->m_isOutput) {
        // trying to connect to the node the other node is connected to
        if (node->m_connected) {
            return connectTo(node->m_connectedNode);
        }
        return;
    }
    if (!m_isOutput) {
        return node->connectTo(this);
    }
    // From this point, this method only runs on outputs.
    NodeBase* inputNode = node;
    if (m_connected) disconnect(true);
    if (inputNode->m_connected) inputNode->disconnect(true);


    // TODO: check for circle
    m_connected = true;
    inputNode->m_connected = true;
    m_connectedNode = node;
    inputNode->m_connectedNode = this;

    setDataBeforeConnect();
    showBezierCurve(m_guiItem, inputNode->m_guiItem, m_guiItem);

    // Notify about changes:
    matrixSizeChanged();
    activeChanged();
    inputNode->dataChanged();
    connectionChanged();
    inputNode->connectionChanged();
}

void NodeBase::disconnect(bool isReconnect)
{
    if (!m_connected) return;
    NodeBase* otherNode = m_connectedNode;
    if (m_isOutput) {
        copyDataBeforeDisconnect();
        if (!isReconnect) setActive(false);
        hideBezierCurve(m_guiItem);
    } else {
        otherNode->copyDataBeforeDisconnect();
        otherNode->setActive(false);
        hideBezierCurve(otherNode->m_guiItem);
    }

    m_connected = false;
    otherNode->m_connected = false;
    m_connectedNode = nullptr;
    otherNode->m_connectedNode = nullptr;

    // other node always dispatches singals
    otherNode->connectionChanged();
    otherNode->activeChanged();
    otherNode->dataChanged();

    // this node only dispatches signals if it is not reconnecting
    // (because then this is done at the end of connect_to() )
    if (!isReconnect) {
        connectionChanged();
        activeChanged();
        dataChanged();
    }
}

void NodeBase::focus()
{
    m_focusExists = true;
    m_focusedNode = this;
}

void NodeBase::defocus()
{
    m_focusExists = false;
}

void NodeBase::updateBezierCurve()
{
    if (!m_isOutput) {
        if (m_connected) {
            return m_connectedNode->updateBezierCurve();
        } else {
            return;
        }
    }
    QQuickItem* bezierCurve = (QQuickItem*)(m_guiItem->findChild<QObject*>("bezierCurve"));
    QMetaObject::invokeMethod(bezierCurve, "update");
}

void NodeBase::setActive(bool value)
{
    if (value == m_active) return;  // value isn't changing
    m_active = value;
    if (m_isOutput) {
        // this connode is always output when this method is called from connect_to()
        for (NodeBase* node: m_dependOnActive) {
            node->setActive(value);
            node->activeChanged();
            node->dataChanged();
        }
    } else if (m_connected) {
        m_connectedNode->setActive(value);
        m_connectedNode->activeChanged();
        m_connectedNode->dataChanged();
    }
}


// -------------------------- HSV Node -------------------------------------------

void OutputNodeHsv::copyDataBeforeDisconnect()
{
	((InputNodeHsv*)m_connectedNode)->data = new ColorMatrix(*m_data);
	m_requestedData = new RequestedData(*(((InputNodeHsv*)m_connectedNode)->m_requestedData));
}


OutputNodeHsv::OutputNodeHsv(BlockInterface* block, int id)
	: NodeBase(true, 1, block, id)
	, m_active(false)
{
	m_data = new ColorMatrix();
	m_requestedData = new RequestedData();
}

void OutputNodeHsv::setDataBeforeConnect()  // wie synchronizAttributes()
{
	auto inputNode = (InputNodeHsv*)m_connectedNode;
	if (m_requestedData != nullptr) {
		delete m_requestedData;
	}
	m_requestedData = inputNode->m_requestedData;
    if (inputNode->data != nullptr) {
        delete inputNode->data;
    }
	inputNode->data = m_data;
    setActive(inputNode->m_active);
}

void OutputNodeHsv::setMatrixSize(size_t sx, size_t sy)
{
    if (m_isOutput) {
		m_data->rescaleTo(sx, sy);
		for (InputNodeHsv* node: m_dependOnMatrixSize) {
            node->setMatrixSize(sx, sy);
            node->matrixSizeChanged();
            node->dataChanged();
        }
    } else {
        if (!m_connected) {
			m_data->rescaleTo(sx, sy);
        } else {
            ((OutputNodeHsv*)m_connectedNode)->setMatrixSize(sx, sy);
            m_connectedNode->matrixSizeChanged();
            m_connectedNode->dataChanged();
        }
    }
}


InputNodeHsv::InputNodeHsv(BlockInterface* block, int id)
	: NodeBase(false, 1, block, id)
	, m_active(false)
{
	m_requestedData = new RequestedData();
	data = new ColorMatrix();
}

void InputNodeHsv::setMatrixSize(size_t sx, size_t sy)
{
	m_requestedData->sx = sx;
	m_requestedData->sy = sy;
}
