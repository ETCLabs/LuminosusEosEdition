#ifndef ARTNETDISCOVERYMANAGER_H
#define ARTNETDISCOVERYMANAGER_H

#include <QUdpSocket>
#include <QVector>
#include <QMap>
#include <QObject>
#include <QTimer>


class ArtNetDiscoveryManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList discoveredNodes READ getDiscoveredNodes NOTIFY discoveredNodesChanged)

public:
    ArtNetDiscoveryManager();

    static QHostAddress getOwnIpAddress();

signals:
    void discoveredNodesChanged();

public slots:

    void processPackets();

    void sendArtPoll();

    void sendArtPollReply();

    QVariantList getDiscoveredNodes() const;

    const QVector<QHostAddress>& getNodeAddresses() const { return m_nodeAddresses; }
    const QVector<QHostAddress>& getUnicastAddresses() const { return m_unicastAddresses; }

private:
    void parseArtPollReply(const QByteArray& content);
    void sendIpCheckPacket();
    void preparePackets();

protected:
    QUdpSocket m_udpSocket;
    QHostAddress m_localAddress;
    QTimer m_artPollTimer;

    QVector<QHostAddress> m_nodeAddresses;
    QVector<QHostAddress> m_unicastAddresses;
    QMap<QString, QVariantMap> m_nodeInfos;

    QByteArray m_ipCheckPacket;
    QByteArray m_artPollPacket;
    QByteArray m_artPollReplyPacket;
};

#endif // ARTNETDISCOVERYMANAGER_H
