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

#include "core/MainController.h"
#include "core/Nodes.h"
#include "core/SmartAttribute.h"

#include <QQmlEngine>
#include <time.h>
#include <string>
#include <cstdlib>
#include <utility>

namespace BlockBaseConstants {
    const QString fallbackQmlFile = "qrc:/qml/FallbackBlockGui.qml";
}

QString getNewUid() {
    return QString::number(time(NULL)).append(QString::number(1000 + std::rand() % 9000));
}

BlockBase::BlockBase(MainController* controller, QString uid)
  : BlockInterface(controller)
  , m_uid(uid)
  , m_controller(controller)
  , m_guiX(0.0)
  , m_guiY(0.0)
  , m_guiWidth(0.0)
  , m_guiHeight(0.0)
  , m_widthIsResizable(false)
  , m_heightIsResizable(false)
  , m_guiItemParent(nullptr)
  , m_guiShouldBeHidden(false)
  , m_focused(false)
  , m_guiItemCompleted(false)
  , m_controllerFunctionCount(1)
  , m_controllerFunctionSelected(0)
  , m_isSceneBlock(false)
  , m_sceneGroup(0)
  , m_lastChangedSceneOrigin(nullptr)
  , m_label(this, "label", "")
{
	// Tell QML that this object is owned by C++ and should not be deleted by the JS GC:
	// This is very important because otherwise SEGFAULTS will appear randomly!
	QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

	if (m_uid.isEmpty()) {
		m_uid = getNewUid();
	}
}

BlockBase::~BlockBase() {
	// delete the created Nodes:
    for (NodeBase* node: m_nodes.values()) {
		Q_ASSERT_X(node, "BlockBase dtor", "Node object does not exist.");
		node->deleteLater();
    }
}

QJsonObject BlockBase::getState() const {
    QJsonObject state;
    state["sceneGroup"] = getSceneGroup();
    state["guiItemHidden"] = guiShouldBeHidden();
    state["group"] = getGroup();
    writeAttributesTo(state);
    getAdditionalState(state);
    return state;
}

void BlockBase::setState(const QJsonObject& state) {
    setSceneGroup(state["sceneGroup"].toInt());
    if (state["guiItemHidden"].toBool()) hideGui();
    setGroup(state["group"].toString());
    readAttributesFrom(state);
    setAdditionalState(state);
}

QJsonArray BlockBase::getConnections() {
    QJsonArray connections;
    for (NodeBase* node: m_nodes.values()) {
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

void BlockBase::disconnectAllNodes() {
    for (NodeBase* node: m_nodes.values()) {
		Q_ASSERT_X(node, "disconnectAllNodes()", "Node object does not exist.");
        node->disconnectAll();
        node->defocus();
	}
}

NodeBase* BlockBase::getDefaultInputNode() {
    for (NodeBase* node: m_nodes.values()) {
		Q_ASSERT_X(node, "getDefaultInputNode()", "Node object does not exist.");
        if (!node->isOutput()) {
			return node;
		}
	}
	return nullptr;
}

NodeBase* BlockBase::getDefaultOutputNode() {
    for (NodeBase* node: m_nodes.values()) {
		Q_ASSERT_X(node, "getDefaultOutputNode()", "Node object does not exist.");
        if (node->isOutput()) {
			return node;
		}
    }
    return nullptr;
}

QList<QPointer<NodeBase> > BlockBase::getNodes() const {
    return m_nodes.values();
}

QJsonObject BlockBase::getNodeMergeModes() const {
    QJsonObject state;
    for (NodeBase* node: m_nodes.values()) {
        int index = node->getIndex();
        if (!node) continue;
        if (!node->isOutput()) {
            state[QString::number(index)] = node->getHtpMode();
        }
    }
    return state;
}

void BlockBase::setNodeMergeModes(const QJsonObject& state) {
    for (NodeBase* node: m_nodes.values()) {
        int index = node->getIndex();
        if (!node) continue;
        if (!node->isOutput()) {
            node->setHtpMode(state[QString::number(index)].toBool());
        }
    }
}

QQuickItem* BlockBase::getGuiItemChild(QString name) {
    if (!m_guiItem) return nullptr;
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
	m_nodes[number] = node;
    m_nodesByName[guiItemName] = node;
    return node;
}

NodeBase* BlockBase::createInputNode(QString guiItemName) {
    int number = m_nodes.size();
    auto node = new NodeBase(this, number, false);
    QQmlEngine::setObjectOwnership(node, QQmlEngine::CppOwnership);
	m_nodes[number] = node;
    m_nodesByName[guiItemName] = node;
    return node;
}

void BlockBase::removeNode(NodeBase* node) {
    if (!node) {
        qCritical() << "BlockBase: Tried to remove nonexisting node.";
        return;
    }
    if (!m_nodes.contains(node->getIndex())) {
        qCritical() << "BlockBase: Tried to remove node of other block.";
        return;
    }
    node->disconnectAll();
    m_nodes.remove(node->getIndex());
    QString name;
    for (QString guiItemName: m_nodesByName.keys()) {
        if (m_nodesByName[guiItemName] == node) {
            name = guiItemName;
            break;
        }
    }
    if (!name.isEmpty()) {
        m_nodesByName.remove(name);
    }
    node->deleteLater();
}

QQuickItem* BlockBase::createQmlItem(QString qmlPath, QQuickItem* parent) {
    if (!parent) return nullptr;
    QQmlComponent* component = new QQmlComponent(m_controller->guiManager()->qmlEngine(), QUrl(qmlPath));
    if (!component) {
        qWarning() << "Couldn't create QML component.";
        return nullptr;
    }
    QQuickItem* newGuiItem = qobject_cast<QQuickItem*>(component->beginCreate(m_controller->guiManager()->qmlEngine()->rootContext()));
    if (!newGuiItem) {
        qCritical() << "Could not create GUI item: " << component->errorString();

        component->deleteLater();
        component = nullptr;
        return nullptr;
    }
    newGuiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
    newGuiItem->setParentItem(parent);
    component->completeCreate();
    component->deleteLater();
    component = nullptr;
    return newGuiItem;
}

void BlockBase::registerAttribute(SmartAttribute* attr) {
    m_blockAttributes[attr->name()] = attr;
    if (attr->persistent()) {
        m_persistentAttributes.append(attr);
    }
}

void BlockBase::setGuiItemCode(QString code) {
    QQmlComponent component(m_controller->guiManager()->qmlEngine());
    component.setData(code.toLatin1(), QUrl(getBlockInfo().qmlFile));
    QQuickItem* newGuiItem = qobject_cast<QQuickItem*>(component.beginCreate(m_controller->guiManager()->qmlEngine()->rootContext()));
    if (!newGuiItem) {
        qCritical() << "Could not create GUI item: " << component.errorString();
        return;
    }
    destroyGuiItem();

    newGuiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
    newGuiItem->setProperty("plane", QVariant::fromValue<QQuickItem*>(m_guiItemParent));
    newGuiItem->setParentItem(m_guiItemParent);
    component.completeCreate();
    newGuiItem->setX(m_guiX);
    newGuiItem->setY(m_guiY);
    if (m_widthIsResizable && m_guiWidth >= 1.0) newGuiItem->setWidth(m_guiWidth);
    if (m_heightIsResizable && m_guiHeight >= 1.0) newGuiItem->setHeight(m_guiHeight);
    m_guiItem = newGuiItem;
    emit positionChangedExternal();  // TODO: is this necessary?

    // TODO: change hidden mechanism?
    connect(m_guiItem, SIGNAL(parentChanged(QQuickItem*)), this, SIGNAL(guiIsHiddenChanged()));

    onGuiItemCreated();
}

void BlockBase::writeAttributesTo(QJsonObject& state) const {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->writeTo(state);
    }
}

void BlockBase::readAttributesFrom(const QJsonObject& state) {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->readFrom(state);
    }
}

QString BlockBase::getBlockName() const { return getBlockInfo().nameInUi; }

void BlockBase::focus() {
    m_controller->blockManager()->focusBlock(this);
}

void BlockBase::defocus() {
    if (getGuiItem()) getGuiItem()->setProperty("focused", false);
    m_focused = false;
    emit focusedChanged();
}

void BlockBase::onFocus() {
	// Qt-independent focus:
    if (getGuiItem()) getGuiItem()->setProperty("focused", true);
    m_focused = true;
    emit focusedChanged();
	// Qt keyboard focus (i.e. for delete key):
    if (getGuiItem()) QMetaObject::invokeMethod(getGuiItem(), "forceActiveFocus");
}

QQmlComponent* BlockBase::getSettingsComponent() const {
    if (!m_guiItem) return nullptr;
    return m_guiItem->property("settingsComponent").value<QQmlComponent*>();
}

void BlockBase::deletedByUser() {
    if (!mayBeRemoved()) return;
    disconnectAllNodes();
    if (getGuiItem() && !guiShouldBeHidden()) {
        QMetaObject::invokeMethod(getGuiItem(), "startDeleteAnimation");
    } else {
        m_controller->blockManager()->deleteBlock(this);
    }
}

void BlockBase::onDeleteAnimationEnd() {
    m_controller->blockManager()->deleteBlock(this);
}

void BlockBase::makeBlocksConnectedToInputsVisible() {
    for (NodeBase* node: m_nodes.values()) {
        if (!node) continue;
        if (node->isOutput()) continue;
        for (NodeBase* outputNode: node->getConnectedNodes()) {
            if (!outputNode) continue;
            BlockInterface* otherBlock = outputNode->getBlock();
            if (!otherBlock) continue;
            if (otherBlock->getGuiItem()) otherBlock->getGuiItem()->setVisible(true);
        }
    }
}

QObject* BlockBase::attr(QString name) {
    if (!m_blockAttributes.contains(name)) {
        qWarning() << "Block has no attribute " << name;
    }
    return static_cast<QObject*>(m_blockAttributes.value(name, nullptr));
}

void BlockBase::createGuiItem() {
    if (m_guiItem) return;

    QQmlComponent* component = new QQmlComponent(m_controller->guiManager()->qmlEngine(), QUrl(this->getBlockInfo().qmlFile));
    if (!component) {
        qWarning() << "Couldn't create QML component.";
        return;
    }
    QQuickItem* newGuiItem = qobject_cast<QQuickItem*>(component->beginCreate(m_controller->guiManager()->qmlEngine()->rootContext()));
    if (!newGuiItem) {
        qCritical() << "Could not create GUI item: " << component->errorString();

        component->deleteLater();
        component = new QQmlComponent(m_controller->guiManager()->qmlEngine(), QUrl(BlockBaseConstants::fallbackQmlFile));
        if (!component) {
            qWarning() << "Could create QML component loader.";
            return;
        }
        newGuiItem = qobject_cast<QQuickItem*>(component->beginCreate(m_controller->guiManager()->qmlEngine()->rootContext()));
        if (!newGuiItem) {
            qCritical() << "Could not create fallback GUI item: " << component->errorString();
            return;
        }
    }
    newGuiItem->setProperty("block", QVariant::fromValue<QObject*>(this));
    newGuiItem->setProperty("plane", QVariant::fromValue<QQuickItem*>(m_guiItemParent));
    if (m_guiShouldBeHidden) {
        newGuiItem->setParentItem(nullptr);
    } else {
        newGuiItem->setParentItem(m_guiItemParent);
    }
    component->completeCreate();
    component->deleteLater();
    component = nullptr;
    newGuiItem->setX(m_guiX);
    newGuiItem->setY(m_guiY);
    if (m_widthIsResizable && m_guiWidth >= 1.0) newGuiItem->setWidth(m_guiWidth);
    if (m_heightIsResizable && m_guiHeight >= 1.0) newGuiItem->setHeight(m_guiHeight);
    m_guiItem = newGuiItem;
    emit positionChangedExternal();  // TODO: is this necessary?

    // TODO: change hidden mechanism?
    connect(m_guiItem, SIGNAL(parentChanged(QQuickItem*)), this, SIGNAL(guiIsHiddenChanged()));

    onGuiItemCreated();
}

void BlockBase::destroyGuiItem(bool immediate) {
    if (!m_guiItem) return;
    m_guiX = m_guiItem->x();
    m_guiY = m_guiItem->y();
    m_guiWidth = m_guiItem->width();
    m_guiHeight = m_guiItem->height();
    m_guiItem->setVisible(false);
    m_guiItem->setParentItem(nullptr);
    if (immediate) {
        delete m_guiItem;
    } else {
        m_guiItem->deleteLater();
    }
    m_guiItem = nullptr;
}

QQuickItem* BlockBase::getGuiItem() {
    return m_guiItem;
}

const QQuickItem* BlockBase::getGuiItemConst() const {
    return m_guiItem;
}

double BlockBase::getGuiX() const {
    return m_guiItem ? m_guiItem->x() : m_guiX;
}

void BlockBase::setGuiX(double value) {
    if (m_guiItem) {
        m_guiItem->setX(value);
        emit positionChangedExternal();
    } else {
        m_guiX = value;
    }
}

double BlockBase::getGuiY() const {
    return m_guiItem ? m_guiItem->y() : m_guiY;
}

void BlockBase::setGuiY(double value) {
    if (m_guiItem) {
        m_guiItem->setY(value);
        emit positionChangedExternal();
    } else {
        m_guiY = value;
    }
}

void BlockBase::moveTo(double x, double y) {
    if (m_guiItem) {
        QMetaObject::invokeMethod(m_guiItem, "moveAnimatedTo", Q_ARG(QVariant, x), Q_ARG(QVariant, y));
    } else {
        setGuiX(x);
        setGuiY(y);
    }
}

double BlockBase::getGuiWidth() const {
    return m_guiItem ? m_guiItem->width() : m_guiWidth;
}

void BlockBase::setGuiWidth(double value) {
    if (value <= 1.0) return;
    if (m_guiItem) {
        m_guiItem->setWidth(value);
        emit positionChangedExternal();
    } else {
        m_guiWidth = value;
    }
}

double BlockBase::getGuiHeight() const {
    return m_guiItem ? m_guiItem->height() : m_guiHeight;
}

void BlockBase::setGuiHeight(double value) {
    if (value <= 1.0) return;
    if (m_guiItem) {
        m_guiItem->setHeight(value);
        emit positionChangedExternal();
    } else {
        m_guiHeight = value;
    }
}

void BlockBase::hideGui() {
    m_guiShouldBeHidden = true;
    QQuickItem* item = getGuiItem();
    if (!item) return;
    m_guiItemParent = item->parentItem();
    item->setParentItem(nullptr);
}

void BlockBase::unhideGui() {
    m_guiShouldBeHidden = false;
    QQuickItem* item = getGuiItem();
    if (!item) return;
    item->setParentItem(m_guiItemParent);
}

bool BlockBase::guiIsHidden() const {
    const QQuickItem* item = getGuiItemConst();
    if (!item) return true;
    return item->parentItem() == nullptr;
}

void BlockBase::setGuiParentItem(QQuickItem* parent) {
    m_guiItemParent = parent;
    if (!m_guiShouldBeHidden && getGuiItem()) {
        getGuiItem()->setProperty("plane", QVariant::fromValue<QQuickItem*>(parent));
        getGuiItem()->setParentItem(parent);
    }
}

// ---------------------------- Scenes ----------------------------

void BlockBase::setSceneData(const void* origin, double factor, const HsvMatrix& data) {
    // TODO: is copy reference to data neccessary?
    m_sceneValues[origin] = {factor, data};
    m_lastChangedSceneOrigin = origin;
    updateFromSceneData();
}

void BlockBase::removeSceneData(const void* origin) {
    m_sceneValues.remove(origin);
    updateFromSceneData();
}

// ------------------------

QObject* BlockBase::node(QString name) {
    if (!m_nodesByName.contains(name)) {
        qWarning() << "Block has no node " << name;
    }
    return static_cast<QObject*>(m_nodesByName.value(name, nullptr));
}

