#include "BlockManager.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "core/SmartAttribute.h"
#include "block_implementations/Luminosus/GroupBlock.h"

#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>


BlockManager::BlockManager(MainController* controller)
    : QObject(dynamic_cast<QObject*>(controller))
    , m_blockList(controller)
    , m_displayedGroup("")
    , m_blocksInDisplayedGroup()
	, m_focusedBlock(nullptr)
    , m_controller(controller)
    , m_startChannel(1)
	, m_lastDeletedBlockStates(BlockManagerConstants::undoHistoryLength)
    , m_hideBlocksOutsideViewports(true)
{
    m_randomConnectionTimer.setInterval(100);
    connect(&m_randomConnectionTimer, SIGNAL(timeout()), this, SLOT(makeRandomConnection()));
	// Register classes which slots should be accessible from QML:
	qmlRegisterType<BlockList>();
	qmlRegisterType<BlockInterface>();
	// Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
	// This is very important because otherwise SEGFAULTS will appear randomly!
	QQmlEngine::setObjectOwnership(&m_blockList, QQmlEngine::CppOwnership);

    m_clickSound.setSource(QUrl("qrc:/sounds/click.wav"));
    m_clickUpSound.setSource(QUrl("qrc:/sounds/clickUp.wav"));
}

NodeBase* BlockManager::getNodeByUid(QString uid) {
    QString blockUid = uid.split("|").at(0);
    int nodeId = uid.split("|").at(1).toInt();
    if (m_currentBlocksByUid.count(blockUid) == 0) return nullptr;
    BlockInterface* block = m_currentBlocksByUid[blockUid];
    return block->getNodeById(nodeId);
}

void BlockManager::updateBlockVisibility(QQuickItem* workspace) {
    if (!m_hideBlocksOutsideViewports) return;
    if (!workspace) return;
    const qreal left = workspace->x() * (-1) - 300;  // offset in negative direction
    const qreal right = left + workspace->width() + 400;
    const qreal top = workspace->y() * (-1) - 300;  // offset in negative direction
    const qreal bottom = top + workspace->height() + 400;

    QVector<BlockInterface*> visibleBlocks;

    // check which block is inside the viewport and should be visible:
    for (BlockInterface* block: m_blocksInDisplayedGroup) {
        if (!block) continue;
        QQuickItem* guiItem = block->getGuiItem();
        if (block->renderIfNotVisible()) {
            if (!guiItem) continue;
            guiItem->setVisible(true);
            continue;
        }
        if (block->guiShouldBeHidden()) continue;

        if (block->getGuiX() > right || (block->getGuiX() + block->getGuiWidth()) < left
                || block->getGuiY() > bottom || (block->getGuiY() + block->getGuiHeight()) < top) {
            // block is not in viewport
            if (!guiItem) continue;
            guiItem->setVisible(false);
        } else {
            // block is inside viewport
            if (!guiItem) {
                block->createGuiItem();
                guiItem = block->getGuiItem();
                if (!guiItem) continue;
            }
            guiItem->setVisible(true);
            visibleBlocks.append(block);
        }
    }

    // iterate over all visible blocks and make the blocks connected to their input nodes
    // also visible because their output nodes are responsible for drawing the connection lines:
    for (BlockInterface* block: visibleBlocks) {
        block->makeBlocksConnectedToInputsVisible();
    }
}

bool BlockManager::isInViewport(QQuickItem* workspace, BlockInterface* block) const {
    if (!workspace) return false;
    const qreal safeZone = 500;
    const qreal left = workspace->x() * (-1) - safeZone;  // offset in negative direction
    const qreal right = left + qMax(workspace->width(), 800.0) + 2*safeZone;
    const qreal top = workspace->y() * (-1) - safeZone;  // offset in negative direction
    const qreal bottom = top + qMax(workspace->height(), 600.0) + 2*safeZone;

    return !(block->getGuiX() > right || (block->getGuiX() + block->getGuiWidth()) < left
                    || block->getGuiY() > bottom || (block->getGuiY() + block->getGuiHeight()) < top);
}

void BlockManager::setHideBlocksOutsideViewports(bool value) {
    m_hideBlocksOutsideViewports = value;

    if (!m_hideBlocksOutsideViewports) {  // FIXME: showing all blocks on double click is slow for many blocks
        for (BlockInterface* block: m_blocksInDisplayedGroup) {
            if (!block) continue;
            QQuickItem* guiItem = block->getGuiItem();
            if (!guiItem) {
                block->createGuiItem();
                guiItem = block->getGuiItem();
                if (!guiItem) continue;
            }
            guiItem->setVisible(true);
        }
    }
}

QString BlockManager::getDisplayedGroupLabel() {
    if (getDisplayedGroup() == "") return "Root";
    BlockInterface* groupBlock = getBlockByUid(getDisplayedGroup());
    if (!groupBlock) {
        return "unknown";
    } else {
        const StringAttribute* label = qobject_cast<StringAttribute*>(groupBlock->attr("label"));
        if (!label) return "unknown";
        if (label->getValue().isEmpty()) return "Group";
        return label->getValue();
    }
}

void BlockManager::setDisplayedGroup(QString group) {
    for (QPointer<BlockInterface>& block: m_blocksInDisplayedGroup) {
        if (block.isNull()) continue;
#ifdef RT_MIDI_AVAILABLE
        // don't destroy, only hide GUI item because MIDI mapping depends on it:
        QQuickItem* guiItem = block->getGuiItem();
        if (guiItem) guiItem->setVisible(false);
#else
        block->destroyGuiItem();
#endif
    }
    m_blocksInDisplayedGroup.clear();

    m_displayedGroup = group;
    for (QPointer<BlockInterface>& block: m_currentBlocks) {
        if (block.isNull()) continue;
        if (block->getGroup() == group) {
            m_blocksInDisplayedGroup.push_back(block);
            emit block->positionChanged();
        }
    }
    updateBlockVisibility(m_controller->guiManager()->getWorkspaceItem());
    emit displayedGroupChanged();
}

void BlockManager::goToParentGroup() {
    if (getDisplayedGroup() == "") return;
    GroupBlock* groupBlock = qobject_cast<GroupBlock*>(getBlockByUid(getDisplayedGroup()));
    if (!groupBlock) {
        setDisplayedGroup("");
    } else {
        groupBlock->showParent();
    }
}

void BlockManager::setGroupOfBlock(BlockInterface* block, QString group) {
    if (block->getGroup() == group) return;
    if (block->getGroup() == getDisplayedGroup()) {
        m_blocksInDisplayedGroup.removeAll(block);
#ifdef RT_MIDI_AVAILABLE
        // don't destroy, only hide GUI item because MIDI mapping depends on it:
        QQuickItem* guiItem = block->getGuiItem();
        if (guiItem) guiItem->setVisible(false);
#else
        block->destroyGuiItem();
#endif
    }
    block->setGroup(group);
    if (group == getDisplayedGroup()) {
        m_blocksInDisplayedGroup.append(block);
        updateBlockVisibility(m_controller->guiManager()->getWorkspaceItem());
    }
}

BlockInterface* BlockManager::restoreBlock(const QJsonObject& blockState, bool animated, bool connectOnAdd) {
	QString blockType = blockState["name"].toString();
	QString uid = blockState["uid"].toString();
	BlockInterface* block = createBlockInstance(blockType, uid);
	if (!block) {
		qWarning() << "Could not create block instance of type: " << blockType;
		return nullptr;
    }
    QJsonObject internalState = blockState["internalState"].toObject();
    // downward comptability for "label":
    if (!blockState["label"].toString().isEmpty()) {
        internalState["label"] = blockState["label"].toString();
    }
    block->setState(internalState);
    block->setNodeMergeModes(blockState["nodeMergeModes"].toObject());

    // determ final position of the block:
    double dp = m_controller->guiManager()->getGuiScaling();
    int finalX = int(blockState["posX"].toDouble() * dp);
    int finalY = int(blockState["posY"].toDouble() * dp);

    // ------ GUI:
    block->setGuiX(finalX);
    block->setGuiY(finalY);
    block->setGuiWidth(blockState["width"].toDouble() * dp);
    block->setGuiHeight(blockState["height"].toDouble() * dp);
    block->setGuiParentItem(m_controller->guiManager()->getWorkspaceItem());
#ifdef RT_MIDI_AVAILABLE
    // always create GUI because MIDI mapping depends on it:
    block->createGuiItem();
    auto guiItem = block->getGuiItem();
    if (guiItem) {
        guiItem->setVisible(false);
        if (block->getGroup() == getDisplayedGroup()) {
            m_blocksInDisplayedGroup.push_back(block);
            if (isInViewport(m_controller->guiManager()->getWorkspaceItem(), block)
                    || block->renderIfNotVisible()) {
                guiItem->setVisible(true);
            }
        }
    }
#else
    if (block->getGroup() == getDisplayedGroup()) {
        m_blocksInDisplayedGroup.push_back(block);
        if (isInViewport(m_controller->guiManager()->getWorkspaceItem(), block)) {
            block->createGuiItem();
        }
    }
    if (block->renderIfNotVisible()) {
        block->createGuiItem();
    }
#endif
    // ------ End GUI

    // "connect on add":
    if (connectOnAdd && NodeBase::focusExists()) {
        NodeBase* focusedNode = NodeBase::getFocusedNode();
        NodeBase* nodeOfAddedBlock = nullptr;
        if (focusedNode->isOutput()) {
            nodeOfAddedBlock = block->getDefaultInputNode();
            finalX = focusedNode->getBlock()->getGuiX() + 50 + focusedNode->getBlock()->getGuiWidth();
            finalY = focusedNode->getBlock()->getGuiY();
        } else {
            nodeOfAddedBlock = block->getDefaultOutputNode();
            finalX = focusedNode->getBlock()->getGuiX() - 50 - block->getGuiWidth();
            finalY = focusedNode->getBlock()->getGuiY();
        }
        if (nodeOfAddedBlock) {
            nodeOfAddedBlock->onTouched();
        }
    }

    // set final position:
    if (animated) {
		// animate to final position:
        QPoint spawn = getSpawnPosition(50);
        block->setGuiX(spawn.x());
        block->setGuiY(spawn.y());
        block->moveTo(finalX, finalY);
	} else {
        block->setGuiX(finalX);
        block->setGuiY(finalY);
	}

	// focus the block if it was previously focused:
	if (blockState["focused"].toBool()) {
		focusBlock(block);
    }

	// return a pointer to the block instance:
	return block;
}

BlockInterface* BlockManager::addNewBlock(QString blockType, int randomOffset) {
	BlockInterface* block = createBlockInstance(blockType);
	if (!block) {
		qWarning() << "Could not create block instance of type: " << blockType;
		return nullptr;
    }

    // ------ GUI:
    QPoint blockListPos = getBlockListPosition();

    // set position before animation:
    block->setGuiX(blockListPos.x());
    block->setGuiY(blockListPos.y());
    block->setGuiParentItem(m_controller->guiManager()->getWorkspaceItem());
    block->setGroup(getDisplayedGroup());
    m_blocksInDisplayedGroup.push_back(block);
    block->createGuiItem();
    // ------ End GUI

    if (randomOffset < 0) {
        randomOffset = BlockManagerConstants::defaultBlockPositionOffset;
    }
    QPoint spawn = getSpawnPosition(randomOffset);
    int finalX = spawn.x() - block->getGuiWidth() / 2;
    int finalY = spawn.y() - block->getGuiHeight() / 2;

	// "connect on add":
    if (NodeBase::focusExists()) {
        NodeBase* focusedNode = NodeBase::getFocusedNode();
        NodeBase* nodeOfAddedBlock = nullptr;
        double dp = m_controller->guiManager()->getGuiScaling();
        if (focusedNode->isOutput()) {
            nodeOfAddedBlock = block->getDefaultInputNode();
            finalX = focusedNode->getBlock()->getGuiX() + 50*dp + focusedNode->getBlock()->getGuiWidth();
            finalY = focusedNode->getBlock()->getGuiY();
		} else {
            nodeOfAddedBlock = block->getDefaultOutputNode();
            finalX = focusedNode->getBlock()->getGuiX() - 50*dp - block->getGuiWidth();
            finalY = focusedNode->getBlock()->getGuiY();
		}
        if (nodeOfAddedBlock) {
            nodeOfAddedBlock->onTouched();
		}
    }

    // animate to final position:
    block->moveTo(finalX, finalY);

    // if there is no other focused block, focus this one:
    if (!getFocusedBlock()) {
        focusBlock(block);
    }

	// return a pointer to the block instance:
    return block;
}

BlockInterface* BlockManager::addBlockByNameQml(QString blockType) {
    return addNewBlock(blockType);
}

void BlockManager::deleteAllBlocks(bool immediate) {
    // iterate over copy because map will be modified:
	for (auto entry: std::map<QString, QPointer<BlockInterface>>(m_currentBlocksByUid)) {
        QString uid = entry.first;
        deleteBlock(uid, /*forced*/ true, /*noRestore*/ true, /*immediate*/ immediate);
    }
}

void BlockManager::deleteBlock(BlockInterface* block, bool forced, bool noRestore, bool immediate) {
	// check if it is allowed to remove the block:
    if (!forced && !block->mayBeRemoved()) return;

    if (!noRestore) {
        // store state to be able to restore it:
        m_lastDeletedBlockStates.append(getBlockState(block));
    }
	// if block has Qt keyboard focus
    if (block->getGuiItemConst() && block->getGuiItemConst()->property("activeFocus").toBool()) {
        m_controller->guiManager()->setKeyboardFocusToWorkspace();
	}

    block->onRemove();
    defocusBlock(block);
    block->disconnectAllNodes();
    block->destroyGuiItem(immediate);
    m_currentBlocks.erase(std::find(m_currentBlocks.begin(), m_currentBlocks.end(), block));
    m_currentBlocksByUid.erase(block->getUid());
    m_blocksInDisplayedGroup.removeAll(block);
    // TODO: check if deleteLater is better (but: blocks have to be deleted before new project is loaded!)
    // deleting it instantly leads to GUI warnings "cannot read property" because block is already deleted
    //block->deleteLater();
    if (immediate) {
        delete block;
    } else {
        block->deleteLater();
    }
    emit blockInstanceCountChanged();
}

void BlockManager::deleteBlock(QString uid, bool forced, bool noRestore, bool immediate) {
    BlockInterface* block = m_currentBlocksByUid[uid];
    deleteBlock(block, forced, noRestore, immediate);
}

void BlockManager::deleteFocusedBlock() {
    if (!m_focusedBlock) return;
    m_focusedBlock->deletedByUser();
}

void BlockManager::duplicateFocusedBlock() {
	if (!m_focusedBlock) return;
	QJsonObject state = getBlockState(m_focusedBlock);
	// remove uid to let the block generate a new one:
	state["uid"] = "";
	// offset the position:
    state["posX"] = state["posX"].toDouble() + (std::rand() % 50);
    state["posY"] = state["posY"].toDouble() + (std::rand() % 50);
    restoreBlock(state, /*animated*/ true, /*connectOnAdd*/ true);

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
    QPoint spawn = getSpawnPosition(50);
    double dp = m_controller->guiManager()->getGuiScaling();
    state["posX"] = spawn.x() / dp;
    state["posY"] = spawn.y() / dp;
    BlockInterface* block = restoreBlock(state, /*animated*/ true, /*connectOnAdd*/ true);
    setGroupOfBlock(block, getDisplayedGroup());
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

QJsonObject BlockManager::getBlockState(BlockInterface* block) const {
    double dp = m_controller->guiManager()->getGuiScaling();
	QJsonObject blockState;
	blockState["name"] = block->getBlockInfo().typeName;
    blockState["uid"] = block->getUid();
    blockState["posX"] = block->getGuiX() / dp;
    blockState["posY"] = block->getGuiY() / dp;
    blockState["width"] = block->getGuiWidth() / dp;
    blockState["height"] = block->getGuiHeight() / dp;
	blockState["focused"] = getFocusedBlock() == block;
    blockState["nodeMergeModes"] = block->getNodeMergeModes();
    blockState["internalState"] = block->getState();
    return blockState;
}

QPoint BlockManager::getBlocksMidpoint() const {
    double avgX = 0;
    double avgY = 0;
    int blockCount = m_blocksInDisplayedGroup.size();

    for (BlockInterface* block: m_blocksInDisplayedGroup) {
        if (!block) continue;
        avgX += (block->getGuiX() + block->getGuiWidth() / 2) / blockCount;
        avgY += (block->getGuiY() + block->getGuiHeight() / 2) / blockCount;
    }
    return QPoint(avgX, avgY);
}

void BlockManager::playClickSound() {
    if (!m_controller->getClickSounds()) return;
    m_clickSound.play();
}

void BlockManager::playClickUpSound() {
    if (!m_controller->getClickSounds()) return;
    m_clickUpSound.play();
}

void BlockManager::destroyAllGuiItemsExcept(BlockInterface* exception) {
    for (BlockInterface* block: m_currentBlocks) {
        if (!block) continue;
        if (block->renderIfNotVisible()) continue;
        if (block == exception) continue;

        block->destroyGuiItem();
    }
}

QPoint BlockManager::getSpawnPosition(int randomOffset) const {
    QQuickWindow* window = m_controller->guiManager()->getMainWindow();
    if (!window) return {0, 0};
    int windowWidth = window->width();
    int windowHeight = window->height();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    int planeX = workspace->x() * -1;
    int planeY = workspace->y() * -1;
	QPoint spawn;
	spawn.setX(planeX + windowWidth / 2);
	spawn.setY(planeY + windowHeight / 2);
	// add a random offset:
    double maxOffset = randomOffset * m_controller->guiManager()->getGuiScaling();
    spawn += QPoint(fmod(std::rand(), maxOffset) - (maxOffset / 2),
                    fmod(std::rand(), maxOffset) - (maxOffset / 2));
	return spawn;
}

QPoint BlockManager::getBlockListPosition() const {
    QQuickWindow* window = m_controller->guiManager()->getMainWindow();
    if (!window) return {0, 0};
    int windowWidth = window->width();
    int windowHeight = window->height();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    int planeX = workspace->x() * -1;
    int planeY = workspace->y() * -1;
	QPoint blockListPos;
	blockListPos.setX(planeX + windowWidth);
	blockListPos.setY(planeY + windowHeight / 2);
	return blockListPos;
}

void BlockManager::focusBlock(BlockInterface* block) {
	if (block == m_focusedBlock) {
		// block already has internal focus, but maybe it lost keyboard focus:
        if (block->getGuiItem()) QMetaObject::invokeMethod(block->getGuiItem(), "forceActiveFocus");
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

void BlockManager::startRandomConnectionTest() {
    m_randomConnectionTimer.start();
}

void BlockManager::stopRandomConnectionTest() {
    m_randomConnectionTimer.stop();
}

void BlockManager::toggleRandomConnectionTest() {
    if (m_randomConnectionTimer.isActive()) {
        m_randomConnectionTimer.stop();
    } else {
        m_randomConnectionTimer.start();
    }
}

void BlockManager::makeRandomConnection() {
    NodeBase* output = nullptr;
    NodeBase* input = nullptr;
    int randOffset = qrand();
    for (std::size_t i = 0; i < m_currentBlocks.size() && !output; ++i) {
        BlockInterface* block = m_currentBlocks.at((randOffset + i) % m_currentBlocks.size());
        QList<QPointer<NodeBase>> nodes = block->getNodes();
        for (int k = 0; k < nodes.size() && !output; ++k) {
            NodeBase* node = nodes.at((randOffset + k) % nodes.size());
            if (node->isOutput()) {
                output = node;
                break;
            }
        }
    }
    if (!output) {
        qDebug() << "Random Connection Test: there is no output node.";
        stopRandomConnectionTest();
        return;
    }
    randOffset = qrand();
    for (std::size_t i = 0; i < m_currentBlocks.size() && !input; ++i) {
        BlockInterface* block = m_currentBlocks.at((randOffset + i) % m_currentBlocks.size());
        QList<QPointer<NodeBase>> nodes = block->getNodes();
        for (int k = 0; k < nodes.size() && !input; ++k) {
            NodeBase* node = nodes.at((randOffset + k) % nodes.size());
            if (!node->isOutput()) {
                input = node;
                break;
            }
        }
    }
    if (!input) {
        qDebug() << "Random Connection Test: there is no input node.";
        stopRandomConnectionTest();
        return;
    }

    qInfo() << "------" << output->getBlock()->getBlockName() << "->" << input->getBlock()->getBlockName();

    if (output->getConnectedNodes().size() >= 2) {
        output->disconnectAll();
    }
    if (input->getConnectedNodes().size() >= 2) {
        input->disconnectAll();
    }
    output->connectTo(input);

    // set edge case values:
    output->setValue(0.0);
    output->setValue(0.712354);
    output->setValue(1.0);
    output->setAbsoluteValue(-234.56);
    input->setRequestedSize(Size(3, 2));
    output->setHsv(1.0, 1.0, 1.0);
    output->setRgb(0.73, 0.26, 0.63);
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
    if (m_currentBlocksByUid.find(block->getUid()) != m_currentBlocksByUid.end()) {
        qWarning() << "Tried to add block with UID already in use.";
        block->deleteLater();
        return nullptr;
    }
	m_currentBlocks.push_back(block);
	m_currentBlocksByUid[block->getUid()] = block;
    emit blockInstanceCountChanged();
	// return a pointer to the block instance:
	return block;
}
