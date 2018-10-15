#include "GroupBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


GroupBlock::GroupBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_parentPosX(this, "parentPosX", 0, -1000000, 1000000)
    , m_parentPosY(this, "parentPosY", 0, -1000000, 1000000)
    , m_groupPosX(this, "groupPosX", 0, -1000000, 1000000)
    , m_groupPosY(this, "groupPosY", 0, -1000000, 1000000)
{

}

void GroupBlock::show() {
    const double dp = m_controller->guiManager()->getGuiScaling();
    if (dp == 0.0) return;
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (!workspace) return;
    m_parentPosX = workspace->x() / dp;
    m_parentPosY = workspace->y() / dp;
    m_controller->guiManager()->setWorkspacePosition(m_groupPosX * dp, m_groupPosY * dp);
    m_controller->blockManager()->setDisplayedGroup(getUid());
    m_controller->guiManager()->setKeyboardFocusToWorkspace();
}

void GroupBlock::showParent() {
    const double dp = m_controller->guiManager()->getGuiScaling();
    if (dp == 0.0) return;
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (!workspace) return;
    m_groupPosX = workspace->x() / dp;
    m_groupPosY = workspace->y() / dp;
    m_controller->guiManager()->setWorkspacePosition(m_parentPosX * dp, m_parentPosY * dp);
    m_controller->blockManager()->setDisplayedGroup(getGroup());
    m_controller->guiManager()->setKeyboardFocusToWorkspace();
}

void GroupBlock::moveFocusedBlockToGroup() {
    BlockInterface* focusedBlock = m_controller->blockManager()->getFocusedBlock();
    if (focusedBlock == nullptr || focusedBlock == this) return;
    if (focusedBlock->getGroup() != getGroup()) return;

    QSet<BlockInterface*> connectedBlocks;
    connectedBlocks.insert(focusedBlock);
    findConnectedBlocks(focusedBlock, connectedBlocks);

    QString group = getUid();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (!workspace) return;
    double xOffset = workspace->x() - m_groupPosX;
    double yOffset = workspace->y() - m_groupPosY;

    for (BlockInterface* block: connectedBlocks) {
        block->setGuiX(block->getGuiX() + xOffset);
        block->setGuiY(block->getGuiY() + yOffset);
        m_controller->blockManager()->setGroupOfBlock(block, group);
    }
}

void GroupBlock::moveAllBlocksToGroup() {
    QSet<BlockInterface*> blocksOnThisLevel;
    QString parentGroup = getGroup();
    for (QPointer<BlockInterface> block: m_controller->blockManager()->getCurrentBlocks()) {
        if (block.isNull()) continue;
        if (block == this) continue;
        if (block->getGroup() == parentGroup) {
            blocksOnThisLevel.insert(block);
        }
    }

    QString group = getUid();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (!workspace) return;
    double xOffset = workspace->x() - m_groupPosX;
    double yOffset = workspace->y() - m_groupPosY;

    for (BlockInterface* block: blocksOnThisLevel) {
        block->setGuiX(block->getGuiX() + xOffset);
        block->setGuiY(block->getGuiY() + yOffset);
        m_controller->blockManager()->setGroupOfBlock(block, group);
    }
}

void GroupBlock::findConnectedBlocks(BlockInterface* block, QSet<BlockInterface*>& connectedBlocks) {
    for (QPointer<NodeBase>& node: block->getNodes()) {
        if (node.isNull()) continue;
        for (const QPointer<NodeBase>& otherNode: node->getConnectedNodes()) {
            if (otherNode.isNull()) continue;
            BlockInterface* otherBlock = otherNode->getBlock();
            if (!otherBlock) continue;
            if (!connectedBlocks.contains(otherBlock)) {
                connectedBlocks.insert(otherBlock);
                findConnectedBlocks(otherBlock, connectedBlocks);
            }
        }
    }
}

void GroupBlock::onRemove() {
    // move all member block to parent group:
    QString group = getUid();
    QString parentGroup = getGroup();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (!workspace) return;
    double xOffset = workspace->x() - m_groupPosX;
    double yOffset = workspace->y() - m_groupPosY;

    for (QPointer<BlockInterface> block: m_controller->blockManager()->getCurrentBlocks()) {
        if (block.isNull()) continue;
        if (block->getGroup() == group) {
            block->setGuiX(block->getGuiX() - xOffset);
            block->setGuiY(block->getGuiY() - yOffset);
            m_controller->blockManager()->setGroupOfBlock(block, parentGroup);
        }
    }
}
