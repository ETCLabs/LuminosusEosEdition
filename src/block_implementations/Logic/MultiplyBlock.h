#ifndef MULTIPLYBLOCK_H
#define MULTIPLYBLOCK_H


#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"

class NodeBase;


class MultiplyBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Multiply";
        info.category << "General" << "Modify Value";
        info.helpText = "Multiplies the values of two inputs.\n\n"
                        "Useful to only send a value when another output is high "
                        "(i.e. multiply a synthetic beat signal with the real volume).";
        info.qmlFile = "qrc:/qml/Blocks/Logic/MultiplyBlock.qml";
        info.complete<MultiplyBlock>();
        return info;
    }

    explicit MultiplyBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void update();

protected:
    QPointer<NodeBase> m_inputNode2;

};

#endif // MULTIPLYBLOCK_H
