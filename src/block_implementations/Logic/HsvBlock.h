#ifndef HSVBLOCK_H
#define HSVBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/Nodes.h"

#include <QPointer>


class HsvBlock : public BlockBase {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "HSV Block";
        info.nameInUi = "Hue + Saturation + Int.";
        info.category << "General" << "Controls";
        info.helpText = "Interprets the input values as Hue, Saturation and Intensity and "
                        "outputs the corresponding color.\n\n"
                        "Useful to apply an effect to the hue value of a color.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/HsvBlock.qml";
        info.complete<HsvBlock>();
        return info;
    }

    HsvBlock(MainController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

protected:
    QPointer<NodeBase> m_outputNode;
    QPointer<NodeBase> m_inputX;
    QPointer<NodeBase> m_inputY;
    QPointer<NodeBase> m_inputZ;

};

#endif // HSVBLOCK_H
