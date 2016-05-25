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
#include "NodeBase.h"

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
{
	// Tell QML that this object is owned by C++ and should not be deleted by the JS GC:
	// This is very important because otherwise SEGFAULTS will appear randomly!
	QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

	if (m_uid.isEmpty()) {
		m_uid = getNewUid();
	}
	m_guiItem = (QQuickItem*)m_component.beginCreate(controller->qmlEngine()->rootContext());
	if (!m_guiItem) {
		qWarning() << "Could not create GUI item: " << m_component.errorString();
    }
    //guiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
}

BlockBase::~BlockBase() {
	// delete the created Nodes:
	for (auto const& entry: m_nodes) {
		NodeBase* node = entry.second;
		node->deleteLater();
	}
}

QJsonArray BlockBase::getConnections() {
	QJsonArray connections;
	for (auto const& kv: m_nodes) {
		NodeBase* node = kv.second;
		if (node->m_isOutput && node->m_connected) {
            // "outputUid->inputUid"
            connections.append(node->getUid().append("->").append(node->m_connectedNode->getUid()));
        }
    }
    return connections;
}

NodeBase* BlockBase::getNodeById(int id) {
	if (m_nodes.count(id) == 0) return nullptr;
	return m_nodes[id];
}

QQuickItem* BlockBase::getGuiItem() {
	Q_ASSERT_X(m_guiItem, "getGuiItem()", "GUI item does not exist");
	m_guiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
	m_component.completeCreate();
	return m_guiItem;
}

const QQuickItem* BlockBase::getGuiItemConst() const {
	Q_ASSERT_X(m_guiItem, "getGuiItem()", "GUI item does not exist");
	return m_guiItem;
}

void BlockBase::disconnectAllNodes() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		node->disconnect();
	}
}

NodeBase* BlockBase::getDefaultInputNode() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		if (!node->m_isOutput) {
			return node;
		}
	}
	return nullptr;
}

NodeBase* BlockBase::getDefaultOutputNode() {
	for (auto entry: m_nodes) {
		NodeBase* node = entry.second;
		if (node->m_isOutput) {
			return node;
		}
	}
	return nullptr;
}

QQuickItem* BlockBase::getGuiItemChild(QString name)
{
	return (QQuickItem*)(m_guiItem->findChild<QObject*>(name));
}

OutputNodeHsv* BlockBase::createOutputNodeHsv(QString guiItemName) {
	int number = m_nodes.size();
    auto node = new OutputNodeHsv(this, number);
    node->setGuiItem(getGuiItemChild(guiItemName));
	m_nodes[number] = node;
    return node;
}

InputNodeHsv* BlockBase::createInputNodeHsv(QString guiItemName) {
	int number = m_nodes.size();
    auto node = new InputNodeHsv(this, number);
    node->setGuiItem(getGuiItemChild(guiItemName));
	m_nodes[number] = node;
    return node;
}

QString BlockBase::getBlockName() const { return getBlockInfo().name; }

void BlockBase::focus() {
	m_controller->blockManager()->focusBlock(this);
}

void BlockBase::defocus() {
    getGuiItem()->setProperty("focused", false);
}

void BlockBase::onFocus() {
	// Qt-independent focus:
	getGuiItem()->setProperty("focused", true);
	// Qt keyboard focus (i.e. for delete key):
	QMetaObject::invokeMethod(getGuiItem(), "forceActiveFocus");
}

QQmlComponent *BlockBase::getSettingsComponent() const {
	return m_guiItem->property("settingsComponent").value<QQmlComponent*>();
}

