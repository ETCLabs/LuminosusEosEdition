#ifndef DMXVALUERANGEBLOCK_H
#define DMXVALUERANGEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"


class DmxValueRangeBlock : public InOutBlock {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "DMX Value Range";
        info.nameInUi = "Slot (i.e. DMX Range)";
        info.category << "General" << "sACN Input";
        info.helpText = "Can be used to trigger something if the input value is between two DMX "
                        "values or at a specific DMX value (by setting upper and lower bound to "
                        "the same value).\n\n"
                        "If 'On / Off only' is enabled, the output is 1 when the input is in the "
                        "range and 0 if not. Otherwise the output will linearly increase between "
                        "the lower and upper bound.\n\n"
                        "Useful in combination with an sACN Input Block to make Luminosus functions "
                        "controllable from a lighting console like a fixture.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/DmxValueRangeBlock.qml";
        info.complete<DmxValueRangeBlock>();
        return info;
    }

    explicit DmxValueRangeBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

protected:
    BoolAttribute m_switch;
    IntegerAttribute m_low;
    IntegerAttribute m_high;
};

#endif // DMXVALUERANGEBLOCK_H
