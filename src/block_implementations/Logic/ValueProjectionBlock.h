#ifndef VALUEPROJECTIONBLOCK_H
#define VALUEPROJECTIONBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"


class ValueProjectionBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Value Projection";
        info.nameInUi = "Scale / Invert";
        info.category << "General" << "Modify Value";
        info.helpText = "Modifies the value range of the input value.\n\n"
                        "By adjusting only the left sliders the input value range is expanded to "
                        "the full output range.\n"
                        "By adjusting only the right sliders the input value range is compressed to a "
                        "part of the output range.\n\n"
                        "By swapping the lower and higher handles, the input can be inverted.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/ValueProjectionBlock.qml";
        info.complete<ValueProjectionBlock>();
        return info;
    }

    explicit ValueProjectionBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

protected:
    DoubleAttribute m_inMin;
    DoubleAttribute m_inMax;
    DoubleAttribute m_outMin;
    DoubleAttribute m_outMax;
};

#endif // VALUEPROJECTIONBLOCK_H
