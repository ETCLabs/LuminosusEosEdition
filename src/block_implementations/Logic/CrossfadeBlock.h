#ifndef CROSSFADEBLOCK_H
#define CROSSFADEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"

class NodeBase;


class CrossfadeBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Crossfade";
        info.category << "General" << "Logic";
        info.helpText = "Crossfades between two inputs.\n"
                        "Colors are faded by mixing their RGB values.\n\n"
                        "The middle input controls the ratio.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/CrossfadeBlock.qml";
        info.complete<CrossfadeBlock>();
        return info;
    }

    explicit CrossfadeBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void update();
    void updateRgb();
    void updateAbsolute();

protected:
    QPointer<NodeBase> m_inputNode2;
    QPointer<NodeBase> m_ratioNode;

    DoubleAttribute m_ratio;

};

#endif // CROSSFADEBLOCK_H
