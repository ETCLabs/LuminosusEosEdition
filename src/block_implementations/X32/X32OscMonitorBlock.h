#ifndef X32OSCMONITORBLOCK_H
#define X32OSCMONITORBLOCK_H

#include "core/block_data/BlockBase.h"


class X32OscMonitorBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "X32 Connection Monitor";
        info.nameInUi = "Connection Monitor";
        info.category << "Behringer X32 / X Air";
        info.helpText = "Shows the OSC messages used for the X32 Connection.\n"
                        "With the buttons at the top you can choose if incoming and / or outgoing "
                        "messages will be logged and displayed.";
        info.qmlFile = "qrc:/qml/Blocks/X32/X32OscMonitorBlock.qml";
        info.complete<X32OscMonitorBlock>();
        return info;
    }

    explicit X32OscMonitorBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // X32OSCMONITORBLOCK_H
