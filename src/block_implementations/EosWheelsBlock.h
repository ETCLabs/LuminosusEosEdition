#ifndef EOSWHEELSBLOCK_H
#define EOSWHEELSBLOCK_H

#include "block_data/BlockBase.h"
#include "utils.h"


class EosWheelsBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Wheels";
        info.nameInUi = "Channel Parameters";
        info.category << "Eos";
        info.helpText = "Shows the available parameters and their values for the active channels.\n\n"
                        "Attention: The shown parameters depend on the selected channels for the "
                        "current OSC User ID (see Eos Debug Info). This may not be the same as "
                        "the console user.";
        info.qmlFile = "qrc:/qml/Blocks/EosWheelsBlock.qml";
        info.complete<EosWheelsBlock>();
        return info;
    }

    explicit EosWheelsBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // EOSWHEELSBLOCK_H
