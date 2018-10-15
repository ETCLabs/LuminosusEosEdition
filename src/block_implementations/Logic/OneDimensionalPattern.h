#ifndef ONEDIMENSIONALPATTERN_H
#define ONEDIMENSIONALPATTERN_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"


class OneDimensionalPattern : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "1D Pattern";
        info.nameInUi = "Bar / Dot";
        info.category << "General" << "Line / Matrix";
        info.helpText = "Only useful with several lights connected in a row or matrix. If the "
                        "input is for example 30% it sets the first 30% of the lights (from left to "
                        "right) to 100%.\n"
                        "Useful for example to visualize an audio level with multiple lights.\n\n"
                        "Setting the pattern type to 'Dot' only sets the light exactly at 30% from "
                        "left to 100% in the given example.\n\n"
                        "The direction can be changed with the 'Rotate / Mirror' block.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/OneDimensionalPattern.qml";
        info.complete<OneDimensionalPattern>();
        return info;
    }

    explicit OneDimensionalPattern(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    void updateRequestedSize();

private:
    void bar();
    void dot();

protected:
    IntegerAttribute m_pattern;

};

#endif // ONEDIMENSIONALPATTERN_H
