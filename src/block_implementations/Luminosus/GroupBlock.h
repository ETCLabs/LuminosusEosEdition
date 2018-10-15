#ifndef GROUPBLOCK_H
#define GROUPBLOCK_H

#include "core/block_data/BlockBase.h"


class GroupBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Group";
        info.nameInUi = "Block Group";
        info.category << "General" << "Groups / Projects";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/GroupBlock.qml";
        info.helpText = "Can be used to hierarchically structure blocks.\n\n"
                        "Add blocks by focusing them and then clicking 'Move to group'. "
                        "All connected blocks will be moved.\n\n"
                        "The blocks can be released from the group by "
                        "deleting this group block (the blocks itself won't be deleted).\n\n"
                        "The label will be displayed at the top of the screen while "
                        "the group is open.";
        info.complete<GroupBlock>();
        return info;
    }

    explicit GroupBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void show();
    void showParent();

    void moveFocusedBlockToGroup();
    void moveAllBlocksToGroup();

    void findConnectedBlocks(BlockInterface* block, QSet<BlockInterface*>& connectedBlocks);

    virtual void onRemove() override;

protected:
    DoubleAttribute m_parentPosX;
    DoubleAttribute m_parentPosY;
    DoubleAttribute m_groupPosX;
    DoubleAttribute m_groupPosY;
};

#endif // GROUPBLOCK_H
