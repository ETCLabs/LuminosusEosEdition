#ifndef CLOCKBLOCK_H
#define CLOCKBLOCK_H

#include "block_data/BlockBase.h"


class ClockBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Clock";
        info.nameInUi = "Clock";
        info.category << "Other";
        info.qmlFile = "qrc:/qml/Blocks/ClockBlock.qml";
        info.complete<ClockBlock>();
        return info;
    }

    explicit ClockBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // CLOCKBLOCK_H
