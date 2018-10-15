#ifndef SMOOTHBLOCK_H
#define SMOOTHBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"
#include "utils.h"


class SmoothBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Smooth";
        info.nameInUi = "Smooth";
        info.category << "General" << "Modify Value";
        info.helpText = "Smooths the incoming value (acts like a lowpass filter).\n"
                        "Up: the amount to smooth the input if it is increasing\n"
                        "Down: the amount to smooth the input if it is decreasing\n\n"
                        "Useful to simulate the heatup and cooldown effect of tungsten / filament "
                        "lights with LEDs.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/SmoothBlock.qml";
        info.complete<SmoothBlock>();
        return info;
    }

    explicit SmoothBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void eachFrame();

    void updateRequestedSize();

protected:
    DoubleAttribute m_heatUpTime;
    DoubleAttribute m_coolDownTime;
    HsvMatrix m_lastValues;
};

#endif // SMOOTHBLOCK_H
