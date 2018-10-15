#ifndef OFFSETBLOCK_H
#define OFFSETBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"


class OffsetBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Offset";
        info.category << "General" << "Line / Matrix";
        info.helpText = "Shifts the input matrix in x and y direction.\n\n"
                        "Connect the offset for exmaple to a Sawtooth Generator to continously "
                        "shift the matrix.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/OffsetBlock.qml";
        info.complete<OffsetBlock>();
        return info;
    }

    explicit OffsetBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void update();

protected:
    DoubleAttribute m_offsetX;
    DoubleAttribute m_offsetY;

};

#endif // OFFSETBLOCK_H
