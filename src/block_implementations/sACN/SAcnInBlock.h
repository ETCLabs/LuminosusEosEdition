#ifndef SACNRECEIVERBLOCK_H
#define SACNRECEIVERBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"

class sACNListener;


class SAcnInBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "sACN In";
        info.category << "General" << "sACN Input";
        info.helpText = "Receives sACN data and outputs the value of a specific channel.\n\n"
                        "In 16 bit mode the first channel is used as the coarse channel and the "
                        "second one as the fine channel.\n\n"
                        "Useful to make Luminosus functions controllable from a lighting "
                        "console like a fixture. Use a 'Slot (i.e. DMX Range) Block' to split one "
                        "channel into multiple ranges for different functions.\n"
                        "See also the 'Virtual Fixture Block'.";
        info.qmlFile = "qrc:/qml/Blocks/sACN/SAcnInBlock.qml";
        info.complete<SAcnInBlock>();
        return info;
    }

    explicit SAcnInBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void onLevelsChanged();

    void updateListener();

protected:
    QSharedPointer<sACNListener> m_listener;

    IntegerAttribute m_universe;
    IntegerAttribute m_channel;
    BoolAttribute m_16bit;
};

#endif // SACNRECEIVERBLOCK_H
