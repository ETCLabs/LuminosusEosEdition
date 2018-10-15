#ifndef X32CHANNELBLOCK_H
#define X32CHANNELBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"
#include "core/Nodes.h"
#include "osc/OSCMessage.h"

#include <QTimer>


class X32ChannelBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(double decibel READ getDecibel NOTIFY decibelChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "X32 Channel";
        info.nameInUi = "Channel";
        info.category << "Behringer X32 / X Air";
        info.helpText = "Controls the selected channel of an X32 console.";
        info.qmlFile = "qrc:/qml/Blocks/X32/X32ChannelBlock.qml";
        info.complete<X32ChannelBlock>();
        return info;
    }

    explicit X32ChannelBlock(MainController* controller, QString uid);

    void setState(const QJsonObject& state) override;

signals:
    void decibelChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void sendFaderPos();
    void sendName();
    void sendPan();
    void sendOn();

    double getDecibel() const;

    void retrieveStateFromConsole();
    void updateSubscription();

    void onMessageReceived(OSCMessage msg);

protected:
    QPointer<NodeBase> m_panNode;
    QPointer<NodeBase> m_onNode;

    IntegerAttribute m_channelNumber;
    IntegerAttribute m_bus;
    DoubleAttribute m_faderPos;
    DoubleAttribute m_pan;
    BoolAttribute m_boost;
    BoolAttribute m_on;

    bool m_pauseValueTransmission;
    QTimer m_subscribeRefreshTimer;
};

#endif // X32CHANNELBLOCK_H
