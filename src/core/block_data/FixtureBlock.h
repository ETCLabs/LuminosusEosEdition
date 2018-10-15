#ifndef FIXTUREBLOCK_H
#define FIXTUREBLOCK_H

#include "core/block_data/InOutBlock.h"


class FixtureBlock : public InOutBlock
{
    Q_OBJECT

public:
    explicit FixtureBlock(MainController* controller, QString uid, int footprint);

public slots:

    void forwardDataToOutput();

    static void forwardData(NodeBase* inputNode, NodeBase* outputNode);

    static void onConnectionChanged(NodeBase* inputNode, NodeBase* outputNode);

    void connectSlots(NodeBase* inputNode, NodeBase* outputNode);

    void notifyAboutAddress();

protected:
    int m_footprint;
    IntegerAttribute m_address;
    DoubleAttribute m_gamma;
};

#endif // FIXTUREBLOCK_H
