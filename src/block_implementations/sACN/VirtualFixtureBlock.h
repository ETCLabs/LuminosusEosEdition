#ifndef VIRTUALFIXTUREBLOCK_H
#define VIRTUALFIXTUREBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"

class sACNListener;


class VirtualFixtureBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Virtual Fixture";
        info.category << "General" << "sACN Input";
        info.helpText = "Receives sACN data and outputs the value of multiple consecutive channels.\n"
                        "The universe can be adjusted in the block settings.\n\n"
                        "Useful to make Luminosus functions controllable from a lighting "
                        "console like a fixture. Use a 'Slot (i.e. DMX Range) Block' to split one "
                        "channel into multiple ranges for different functions.";
        info.qmlFile = "qrc:/qml/Blocks/sACN/VirtualFixtureBlock.qml";
        info.complete<VirtualFixtureBlock>();
        return info;
    }

    explicit VirtualFixtureBlock(MainController* controller, QString uid);

signals:
    void channelNodesUpdated();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    NodeBase* getChannelNode(int index);

private slots:
    void onLevelsChanged();

    void updateListener();

    void updateNodeCount();

protected:
    QSharedPointer<sACNListener> m_listener;

    IntegerAttribute m_universe;
    IntegerAttribute m_channel;
    IntegerAttribute m_numChannels;
    IntegerAttribute m_actualChannelCount;

    QVector<NodeBase*> m_channelNodes;
};

#endif // VIRTUALFIXTUREBLOCK_H
