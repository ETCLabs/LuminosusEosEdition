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

#include "BlockBase.h"

#include "MainController.h"
#include "Nodes.h"

#include <QQmlEngine>
#include <time.h>
#include <string>
#include <cstdlib>

QString getNewUid() {
    return QString::number(time(NULL)).append(QString::number(1000 + std::rand() % 9000));
}

BlockBase::BlockBase(MainController* controller, QString uid, QString qmlUrl)
  : BlockInterface(controller)
  , m_uid(uid)
  , m_controller(controller)
  , m_component(controller->qmlEngine(), QUrl(qmlUrl))
  , m_focused(false)
  , m_guiItemCompleted(false)
{
	// Tell QML that this object is owned by C++ and should not be deleted by the JS GC:
	// This is very important because otherwise SEGFAULTS will appear randomly!
	QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

	if (m_uid.isEmpty()) {
		m_uid = getNewUid();
	}
	m_guiItem = (QQuickItem*)m_component.beginCreate(controller->qmlEngine()->rootContext());
	if (!m_guiItem) {
		qCritical() << "Could not create GUI item: " << m_component.errorString();
	}
    //guiItem->setProperty("block", QVariant::fromValue<QObject*>(this)); -> called in completeCreation()
}

BlockBase::~BlockBase() {
	// delete the created Nodes:
	for (auto const& entry: m_nodes) {
		NodeBase* node = entry.second;
		Q_ASSERT_X(node, "BlockBase dtor", "Node object does not exist.");
		node->deleteLater();
    }
}

void BlockBase::completeGuiItemCreation() {
    m_guiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
    m_component.completeCreate();
    m_guiItemCompleted = true;
}

QJsonArray BlockBase::getConnections() {
	QJsonArray connections;
	for (auto const& kv: m_nodes) {
		NodeBase* node = kv.second;
		Q_ASSERT_X(node, "getConnections()", "Node object does not exist.");
        if (node->isOutput()) {
            for (QString connection: node->getConnections())
            connections.append(connection);
            // "outputUid->inputUid"
            // node->getUid().append("->").append(node->m_connectedNode->getUid())
        }
    }
    return connections;
}

NodeBase* BlockBase::getNodeById(int id) {
	if (m_nodes.count(id) == 0) return nullptr;
	return m_nodes[id];
}

QQuickItem* BlockBase::getGuiItem() {
    Q_ASSERT_X(m_guiItem, "getGuiItem()", "GUI item does not exist.");
    if (!m_guiItemCompleted) {
        completeGuiItemCreation();
    }
	return m_guiItem;
}

const QQuickItem* BlockBase::getGuiItemConst() const {
	Q_ASSERT_X(m_guiItem, "getGuiItemConst()", "GUI item does not exist.");
	return m_guiItem;
}

void BlockBase::disconnectAllNodes() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		Q_ASSERT_X(node, "disconnectAllNodes()", "Node object does not exist.");
        node->disconnectAll();
        node->defocus();
	}
}

NodeBase* BlockBase::getDefaultInputNode() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		Q_ASSERT_X(node, "getDefaultInputNode()", "Node object does not exist.");
        if (!node->isOutput()) {
			return node;
		}
	}
	return nullptr;
}

NodeBase* BlockBase::getDefaultOutputNode() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		Q_ASSERT_X(node, "getDefaultOutputNode()", "Node object does not exist.");
        if (node->isOutput()) {
			return node;
		}
	}
    return nullptr;
}

QJsonObject BlockBase::getNodeMergeModes() const {
    QJsonObject state;
    for (auto entry: m_nodes) {
        int index = entry.first;
        NodeBase* node = entry.second;
        if (!node) continue;
        if (!node->isOutput()) {
            state[QString::number(index)] = node->getHtpMode();
        }
    }
    return state;
}

void BlockBase::setNodeMergeModes(const QJsonObject& state) {
    for (auto entry: m_nodes) {
        int index = entry.first;
        NodeBase* node = entry.second;
        if (!node) continue;
        if (!node->isOutput()) {
            node->setHtpMode(state[QString::number(index)].toBool());
        }
    }
}

QQuickItem* BlockBase::getGuiItemChild(QString name) {
	Q_ASSERT_X(m_guiItem, "getGuiItemChild()", "GUI item does not exist");
	QQuickItem* item = qobject_cast<QQuickItem*>(m_guiItem->findChild<QObject*>(name));
	if (!item) {
		qCritical() << "Could not find GUI item with object name: " << name;
		return nullptr;
	}
	return item;
}

NodeBase* BlockBase::createOutputNode(QString guiItemName) {
	int number = m_nodes.size();
    auto node = new NodeBase(this, number, true);
	QQmlEngine::setObjectOwnership(node, QQmlEngine::CppOwnership);
    node->setGuiItem(getGuiItemChild(guiItemName));
	m_nodes[number] = node;
    return node;
}

NodeBase* BlockBase::createInputNode(QString guiItemName) {
	int number = m_nodes.size();
    auto node = new NodeBase(this, number, false);
	QQmlEngine::setObjectOwnership(node, QQmlEngine::CppOwnership);
    node->setGuiItem(getGuiItemChild(guiItemName));
	m_nodes[number] = node;
    return node;
}

QString BlockBase::getBlockName() const { return getBlockInfo().nameInUi; }

void BlockBase::focus() {
    m_controller->blockManager()->focusBlock(this);
}

void BlockBase::defocus() {
    getGuiItem()->setProperty("focused", false);
    m_focused = false;
    emit focusedChanged();
}

void BlockBase::onFocus() {
	// Qt-independent focus:
	getGuiItem()->setProperty("focused", true);
    m_focused = true;
    emit focusedChanged();
	// Qt keyboard focus (i.e. for delete key):
	QMetaObject::invokeMethod(getGuiItem(), "forceActiveFocus");
}

QQmlComponent* BlockBase::getSettingsComponent() const {
	Q_ASSERT_X(m_guiItem, "getGuiItemChild()", "GUI item does not exist");
    return m_guiItem->property("settingsComponent").value<QQmlComponent*>();
}

void BlockBase::deletedByUser() {
    disconnectAllNodes();
    QMetaObject::invokeMethod(getGuiItem(), "startDeleteAnimation");
}

void BlockBase::onDeleteAnimationEnd() {
    m_controller->blockManager()->deleteBlock(this);
}

void BlockBase::makeBlocksConnectedToInputsVisible() {
    for (auto entry: m_nodes) {
        NodeBase* node = entry.second;
        if (!node) continue;
        if (node->isOutput()) continue;
        for (NodeBase* outputNode: node->getConnectedNodes()) {
            if (!outputNode) continue;
            BlockInterface* otherBlock = outputNode->getBlock();
            if (!otherBlock) continue;
            otherBlock->getGuiItem()->setVisible(true);
        }
    }
}

