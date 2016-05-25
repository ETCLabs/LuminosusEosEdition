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

#include "BlockManager.h"

#include "MainController.h"
#include "NodeBase.h"

#include <QQmlEngine>
#include <QQuickItem>
#include <QWindow>

BlockManager::BlockManager(MainController* controller)
    : QObject(dynamic_cast<QObject*>(controller))
	, m_focusedBlock(nullptr)
    , m_controller(controller)
	, m_startChannel(1)
	, m_lastDeletedBlockStates(BlockManagerConstants::undoHistoryLength)
{
	// Register classes which slots should be accessible from QML:
	qmlRegisterType<BlockList>();
	qmlRegisterType<BlockInterface>();
	// Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
	// This is very important because otherwise SEGFAULTS will appear randomly!
	QQmlEngine::setObjectOwnership(&m_blockList, QQmlEngine::CppOwnership);
}

NodeBase* BlockManager::getNodeByUid(QString uid) {
    QString blockUid = uid.split("|").at(0);
    int nodeId = uid.split("|").at(1).toInt();
    if (m_currentBlocksByUid.count(blockUid) == 0) return nullptr;
    BlockInterface* block = m_currentBlocksByUid[blockUid];
	return block->getNodeById(nodeId);
}

BlockInterface* BlockManager::restoreBlock(const QJsonObject& blockState, bool animated) {
	QString blockType = blockState["name"].toString();
	QString uid = blockState["uid"].toString();
	BlockInterface* block = createBlockInstance(blockType, uid);
	if (!block) {
		qWarning() << "Could not create block instance of type: " << blockType;
		return nullptr;
	}
	block->setState(blockState["internalState"].toObject());
	QQuickItem* guiItem = createGuiItem(block);
	if (!guiItem) {
		qWarning() << "Could not create GUI item for block type: " << blockType;
		return nullptr;
	}

	// final position of the block:
	int finalX = blockState["posX"].toInt();
	int finalY = blockState["posY"].toInt();
	if (animated) {
		// set position before animation:
		QPoint spawn = getSpawnPosition();
		guiItem->setX(spawn.x());
		guiItem->setY( spawn.y());
		// animate to final position:
		QMetaObject::invokeMethod(guiItem, "moveAnimatedTo", Q_ARG(QVariant, finalX), Q_ARG(QVariant, finalY));
	} else {
		guiItem->setX(finalX);
		guiItem->setY(finalY);
	}

	// focus the block if it was previously focused:
	if (blockState["focused"].toBool()) {
		focusBlock(block);
	}

	// return a pointer to the block instance:
	return block;
}

BlockInterface* BlockManager::addNewBlock(QString blockType) {
	BlockInterface* block = createBlockInstance(blockType);
	if (!block) {
		qWarning() << "Could not create block instance of type: " << blockType;
		return nullptr;
	}
	QQuickItem* guiItem = createGuiItem(block);
	if (!guiItem) {
		qWarning() << "Could not create GUI item for block type: " << blockType;
		return nullptr;
	}

	// set position before animation:
	QPoint blockListPos = getBlockListPosition();
	guiItem->setX(blockListPos.x());
	guiItem->setY( blockListPos.y());
	// animate to final position:
	QPoint spawn = getSpawnPosition();
	int finalX = spawn.x() - guiItem->width() / 2;
	int finalY = spawn.y() - guiItem->height() / 2;

	// "connect on add":
	if (NodeBase::m_focusExists) {
		NodeBase* focusedNode = NodeBase::m_focusedNode;
		QQuickItem* otherGuiItem = focusedNode->m_block->getGuiItem();
		NodeBase* otherNode = nullptr;
		if (focusedNode->m_isOutput) {
			otherNode = block->getDefaultInputNode();
			finalX = otherGuiItem->x() - 50 - guiItem->width();
			finalY = otherGuiItem->y();
		} else {
			otherNode = block->getDefaultOutputNode();
			finalX = otherGuiItem->x() + 50 + otherGuiItem->width();
			finalY = otherGuiItem->y();
		}
		if (otherNode) {
			otherNode->onTouched();
		}
	}

	// if there is no other focused block, focus this one:
	if (!getFocusedBlock()) {
		focusBlock(block);
	}

	QMetaObject::invokeMethod(guiItem, "moveAnimatedTo", Q_ARG(QVariant, finalX), Q_ARG(QVariant, finalY));

	// return a pointer to the block instance:
    return block;
}

void BlockManager::addBlockByNameQml(QString blockType) {
	addNewBlock(blockType);
}

void BlockManager::deleteAllBlocks() {
    // iterate over copy because map will be modified:
    for (auto entry: std::map<QString, BlockInterface*>(m_currentBlocksByUid)) {
        QString uid = entry.first;
        deleteBlock(uid, true);
    }
}

void BlockManager::deleteBlock(BlockInterface* block, bool forced) {
	// check if it is allowed to remove the block:
    if (!forced && !block->mayBeRemoved()) return;

	// store state to be able to restore it:
	m_lastDeletedBlockStates.append(getBlockState(block));
	// if block has Qt keyboard focus
	if (block->getGuiItem()->property("activeFocus").toBool()) {
		m_controller->setKeyboardFocusToPlane();
	}

    block->onRemove();
    defocusBlock(block);
    block->disconnectAllNodes();
    QQuickItem* guiItem = block->getGuiItem();
	if (guiItem) {
        guiItem->setProperty("visible", false);
        guiItem->setParentItem(NULL);
		guiItem->deleteLater();
    }
    m_currentBlocks.erase(std::find(m_currentBlocks.begin(), m_currentBlocks.end(), block));
    m_currentBlocksByUid.erase(block->getUid());
    block->deleteLater();
}

void BlockManager::deleteBlock(QString uid, bool forced) {
    BlockInterface* block = m_currentBlocksByUid[uid];
    deleteBlock(block, forced);
}

void BlockManager::deleteFocusedBlock() {
    if (!m_focusedBlock) return;
	deleteBlock(m_focusedBlock);
}

void BlockManager::duplicateFocusedBlock() {
	if (!m_focusedBlock) return;
	QJsonObject state = getBlockState(m_focusedBlock);
	// remove uid to let the block generate a new one:
	state["uid"] = "";
	// offset the position:
	state["posX"] = state["posX"].toInt() + (std::rand() % 50);
	state["posY"] = state["posY"].toInt() + (std::rand() % 50);
	restoreBlock(state);
}

void BlockManager::copyFocusedBlock() {
	if (!m_focusedBlock) return;
	m_copiedBlockState = getBlockState(m_focusedBlock);
}

void BlockManager::pasteBlock() {
	if (m_copiedBlockState.isEmpty()) return;
	QJsonObject state = m_copiedBlockState;
	// remove uid to let the block generate a new one:
	state["uid"] = "";
	// set position to normal spawn position:
	QPoint spawn = getSpawnPosition();
	state["posX"] = spawn.x();
	state["posY"] = spawn.y();
	restoreBlock(state);
}

void BlockManager::restoreDeletedBlock() {
	if (m_lastDeletedBlockStates.isEmpty()) {
		return;
	}
	QJsonObject state = m_lastDeletedBlockStates.last();
	m_lastDeletedBlockStates.pop_back();
	restoreBlock(state);
}

BlockInterface* BlockManager::getBlockByUid(QString uid) {
	if (m_currentBlocksByUid.find(uid) != m_currentBlocksByUid.end()) {
		return m_currentBlocksByUid[uid];
	}
	return nullptr;
}

QJsonObject BlockManager::getBlockState(BlockInterface* block) {
	QJsonObject blockState;
	blockState["name"] = block->getBlockInfo().name;
	blockState["uid"] = block->getUid();
	blockState["posX"] = block->getGuiItem()->x();
	blockState["posY"] = block->getGuiItem()->y();
	blockState["focused"] = getFocusedBlock() == block;
	blockState["internalState"] = block->getState();
	return blockState;
}

QPoint BlockManager::getSpawnPosition() {
	QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
	int windowWidth = rootObject->property("width").toInt();
	int windowHeight = rootObject->property("height").toInt();
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	int planeX = plane->x() * -1;
	int planeY = plane->y() * -1;
	QPoint spawn;
	spawn.setX(planeX + windowWidth / 2);
	spawn.setY(planeY + windowHeight / 2);
	// add a random offset:
	spawn += QPoint(std::rand() % 50, std::rand() % 50);
	return spawn;
}

QPoint BlockManager::getBlockListPosition() {
	QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
	int windowWidth = rootObject->property("width").toInt();
	int windowHeight = rootObject->property("height").toInt();
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	int planeX = plane->x() * -1;
	int planeY = plane->y() * -1;
	QPoint blockListPos;
	blockListPos.setX(planeX + windowWidth);
	blockListPos.setY(planeY + windowHeight / 2);
	return blockListPos;
}

void BlockManager::focusBlock(BlockInterface* block) {
	if (block == m_focusedBlock) {
		// block already has internal focus, but maybe it lost keyboard focus:
		QMetaObject::invokeMethod(block->getGuiItem(), "forceActiveFocus");
		return;
	}
    if (m_focusedBlock) {
        m_focusedBlock->defocus();
    }
    block->onFocus();
    m_focusedBlock = block;
    focusChanged();
}

void BlockManager::defocusBlock(BlockInterface* block) {
    if (block != m_focusedBlock) return;
    block->defocus();
    m_focusedBlock = nullptr;
	focusChanged();
}

BlockInterface* BlockManager::createBlockInstance(QString blockType, QString uid) {
	// check if block type is available:
	if (!m_blockList.blockExists(blockType)) {
		qWarning() << "Block type does not exist: " << blockType;
		return nullptr;
	}
	// create an instance:
	BlockInterface* block = m_blockList.getBlockInfoByName(blockType).createInstanceOnHeap(m_controller, uid);
	// add instance to lists:
	m_currentBlocks.push_back(block);
	m_currentBlocksByUid[block->getUid()] = block;
	// return a pointer to the block instance:
	return block;
}

QQuickItem*BlockManager::createGuiItem(BlockInterface* block) {
	// create GUI item:
	QQuickItem* guiItem = block->getGuiItem();
	QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	// set the "plane" property:
	guiItem->setProperty("plane", QVariant::fromValue<QQuickItem*>(plane));
	// set the GUI parent:
	guiItem->setParentItem(plane);
	return guiItem;
}
