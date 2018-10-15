#ifndef VERTICALPATCHBLOCK_H
#define VERTICALPATCHBLOCK_H

#include "core/block_data/BlockBase.h"
#include "utils.h"

#include "core/Nodes.h"


class VerticalPatchBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Vertical Patch";
        info.nameInUi = "Join Rows";
        info.category << "General" << "Line / Matrix";
        info.helpText = "Joins two rows of blocks (i.e. Eos Channels) to build a matrix.\n"
                        "More than two rows can be joined by using multiple of this blocks.\n\n"
                        "Useful to duplicate a linear effect to multiple rows or to create "
                        "two dimensional effects (i.e. diagonal shifts or gradients).";
        info.qmlFile = "qrc:/qml/Blocks/Theater/VerticalPatchBlock.qml";
        info.complete<VerticalPatchBlock>();
        return info;
    }

    explicit VerticalPatchBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void updateOutput();
    void updateRequestedSize();

protected:
    QPointer<NodeBase> m_inputNode;
    QPointer<NodeBase> m_outputNode1;
    QPointer<NodeBase> m_outputNode2;
};

#endif // VERTICALPATCHBLOCK_H
