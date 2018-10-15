#include "ArtNetDiscoveryManager.h"

#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QDebug>

const int ARTNET_PORT = 6454;

namespace ArtNetConstants {
    static QByteArray PacketBegin = QString("Art-Net").toLatin1().append((char)0x00);
}

inline uint8_t lowByte(uint16_t value) {
    return uint8_t(value & 0xff);
}

inline uint8_t highByte(uint16_t value) {
    return uint8_t(value >> 8);
}


ArtNetDiscoveryManager::ArtNetDiscoveryManager()
    : QObject(nullptr)
    , m_localAddress(QHostAddress::Null)
{
    preparePackets();

    m_udpSocket.bind(ARTNET_PORT);
    connect(&m_udpSocket, SIGNAL(readyRead()), this, SLOT(processPackets()));

    sendIpCheckPacket();

    m_artPollTimer.setInterval(3000);
    connect(&m_artPollTimer, SIGNAL(timeout()), this, SLOT(sendArtPoll()));
    m_artPollTimer.start();
}

QHostAddress ArtNetDiscoveryManager::getOwnIpAddress() {
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for (const QHostAddress& addr: list) {
        if (addr.isLoopback()) continue;
        if (addr.protocol() != QAbstractSocket::IPv4Protocol) continue;
        if (addr == QHostAddress::Any) continue;
        return addr;
    }
    return QHostAddress::Null;
}

void ArtNetDiscoveryManager::processPackets() {
    if (!m_udpSocket.hasPendingDatagrams()) return;
    QNetworkDatagram datagram = m_udpSocket.receiveDatagram();

    // ignore own packets:
    if (datagram.senderAddress() == m_localAddress) return;

    QByteArray content = datagram.data();

    if (content == m_ipCheckPacket) {
        m_localAddress = datagram.senderAddress();
        return;
    }

    if (!content.startsWith(ArtNetConstants::PacketBegin)) return;
    // ArtNet packets have at least the length 10 (Begin + 2 bytes OpCode):
    if (content.size() < 10) return;

    // -> every packet at this point is an ArtNet packet from another system


    // ArtDMX:
    if (content.at(8) == lowByte(0x5000) && content.at(9) == highByte(0x5000)) {
        return;
    }

    // ArtPoll:
    if (content.at(8) == lowByte(0x2000) && content.at(9) == highByte(0x2000)) {
        sendArtPollReply();
        return;
    }

    // ArtPollReply:
    if (content.at(8) == lowByte(0x2100) && content.at(9) == highByte(0x2100)) {
        parseArtPollReply(content);
        return;
    }



}

void ArtNetDiscoveryManager::sendArtPoll() {
    m_udpSocket.writeDatagram(m_artPollPacket, QHostAddress::Broadcast, ARTNET_PORT);
    // each node has to answer to this poll, this node too:
    sendArtPollReply();
}

void ArtNetDiscoveryManager::sendArtPollReply() {
    m_udpSocket.writeDatagram(m_artPollReplyPacket, QHostAddress::Broadcast, ARTNET_PORT);
}

QVariantList ArtNetDiscoveryManager::getDiscoveredNodes() const {
    QVariantList list;
    for (const QHostAddress& addr: m_nodeAddresses) {
        list.append(m_nodeInfos.value(addr.toString()));
    }
    return list;
}

void ArtNetDiscoveryManager::parseArtPollReply(const QByteArray& content) {
    if (content.size() < 17) return;

    QString ipString = QString("%1.%2.%3.%4").arg(uint8_t(content.at(10))).arg(uint8_t(content.at(11)))
                                .arg(uint8_t(content.at(12))).arg(uint8_t(content.at(13)));
    QHostAddress address(ipString);

    // check if node is already known:
    if (m_nodeAddresses.contains(address)) return;

    QString shortName = QString::fromLatin1(static_cast<const char*>(content.begin() + 26));

    QString longName = QString::fromLatin1(static_cast<const char*>(content.begin() + 44));

    QString nodeReport = QString::fromLatin1(static_cast<const char*>(content.begin() + 108));

    int net = 0;
    int subnet = 0;
    int universe1 = 0;
//    int universe2 = 0;
//    int universe3 = 0;
//    int universe4 = 0;
    int numPorts = 0;

    if (content.size() >= 236) {
        net = uint8_t(content.at(18));
        subnet = uint8_t(content.at(19));
        universe1 = uint8_t(content.at(190) & 0b00001111);
//        universe2 = uint8_t(content.at(191) & 0b00001111);
//        universe3 = uint8_t(content.at(192) & 0b00001111);
//        universe4 = uint8_t(content.at(193) & 0b00001111);
        numPorts = uint8_t(content.at(173));
    }

    QVariantMap map;
    map["ipString"] = ipString;
    map["shortName"] = shortName;
    map["longName"] = longName;
    map["nodeReport"] = nodeReport;
    map["net"] = net;
    map["subnet"] = subnet;
    map["universe1"] = universe1;
    map["numPorts"] = numPorts;

    m_nodeAddresses.append(address);
    if (numPorts > 0) {
        m_unicastAddresses.append(address);
    }
    m_nodeInfos[address.toString()] = map;
    emit discoveredNodesChanged();
}

void ArtNetDiscoveryManager::sendIpCheckPacket() {
    m_ipCheckPacket = (QString("LuminosusIpCheck") + QString::number(qrand() % 99999)).toLatin1();
    m_udpSocket.writeDatagram(m_ipCheckPacket, QHostAddress::Broadcast, ARTNET_PORT);
}

void ArtNetDiscoveryManager::preparePackets() {
    // ArtPoll packet:
    m_artPollPacket.append(ArtNetConstants::PacketBegin);
    // OpCode 0x2000
    m_artPollPacket.append(lowByte(0x2000));
    m_artPollPacket.append(highByte(0x2000));
    // Protocol Version 14
    m_artPollPacket.append(highByte(14));
    m_artPollPacket.append(lowByte(14));
    // TalkToMe (sets node behavior, default (0) is ok)
    m_artPollPacket.append((char)0b00000000);
    // Priority
    m_artPollPacket.append((char)0);

    //qDebug() << "ArtPoll: " << m_artPollPacket.size() << m_artPollPacket;
    //Q_ASSERT_X(m_artPollPacket.size() == 14, "ArtNetDiscoveryManager::preparePackets()", "ArtPoll is wrong");

    // ArtPollReply packet:
    m_artPollReplyPacket.append(ArtNetConstants::PacketBegin);
    // OpCode 0x2100
    m_artPollReplyPacket.append(lowByte(0x2100));
    m_artPollReplyPacket.append(highByte(0x2100));
    // IP Address
    quint32 ipAddr = getOwnIpAddress().toIPv4Address();
    m_artPollReplyPacket.append(uint8_t(ipAddr >> 24));
    m_artPollReplyPacket.append(uint8_t(ipAddr >> 16));
    m_artPollReplyPacket.append(uint8_t(ipAddr >> 8));
    m_artPollReplyPacket.append(uint8_t(ipAddr));
    // Port (always 0x1936)
    m_artPollReplyPacket.append(lowByte(0x1936));
    m_artPollReplyPacket.append(highByte(0x1936));
    // Firmware Version
    m_artPollReplyPacket.append(highByte(10));
    m_artPollReplyPacket.append(lowByte(10));
    // set all remaining bytes to zero because as this isn't an ArtNet Node but a controller
    // the rest doesn't make much sense
    m_artPollReplyPacket.append(8, 0x00);
    // Short Name (18 byte)
    QByteArray shortName = QString("Luminosus").toLatin1();
    m_artPollReplyPacket.append(shortName);
    m_artPollReplyPacket.append(18 - shortName.size(), 0x00);
    // Long Name (64 byte)
    QByteArray longName = QString("Luminosus - Stage Multitool").toLatin1();
    m_artPollReplyPacket.append(longName);
    m_artPollReplyPacket.append(64 - longName.size(), 0x00);
    // set all remaining bytes to zero because as this isn't an ArtNet Node but a controller
    // the rest doesn't make much sense
    m_artPollReplyPacket.append(128, 0x00);

    //qDebug() << "ArtPollReply: " << m_artPollReplyPacket.size() << m_artPollReplyPacket;
    //Q_ASSERT_X(m_artPollReplyPacket.size() == 236, "ArtNetDiscoveryManager::preparePackets()", "ArtPollReply is wrong");
}
