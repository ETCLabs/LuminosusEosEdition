#ifndef COLORIZEBLOCK_H
#define COLORIZEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"


class ColorizeBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Colorize";
        info.nameInUi = "Gradient";
        info.category << "General" << "Line / Matrix";
        info.helpText = "Applies a color gradient to the incoming values.\n\n"
                        "A 'Pixel Control Block' connected to the upper input "
                        "is recommended to define the step colors.\n\n"
                        "The lower input controls the intensity.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/ColorizeBlock.qml";
        info.complete<ColorizeBlock>();
        return info;
    }

    explicit ColorizeBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }


private slots:
    void updateOutput();
    void updateRequestedSize();

protected:
    QPointer<NodeBase> m_stepsNode;

    IntegerAttribute m_stepCount;
    BoolAttribute m_smooth;
    BoolAttribute m_horizontal;

};

#endif // COLORIZEBLOCK_H
