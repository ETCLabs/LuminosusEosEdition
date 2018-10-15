#include "ArtNetSender.h"

#include <QtNetwork>
#include <algorithm>

#include "utils.h"


const int ARTNET_PORT = 6454;

inline uint8_t lowByte(uint16_t value) {
    return uint8_t(value & 0xff);
}

inline uint8_t highByte(uint16_t value) {
    return uint8_t(value >> 8);
}


ArtNetSubnetSender::ArtNetSubnetSender(int net, int subnet)
    : m_sequenceNumber(0)
{
    preparePackets(net, subnet);
}

void ArtNetSubnetSender::setNetAndSubnet(int net, int subnet) {
    preparePackets(net, subnet);
}

void ArtNetSubnetSender::sendUniverseBroadcast(int universe, const QVector<uint8_t>& data) {
    Q_ASSERT_X(universe <= 15, "ArtNetSubnetSender::sendUniverse", "Universe number too large");
    Q_ASSERT_X(universe >= 0, "ArtNetSubnetSender::sendUniverse", "Universe number too small");
    Q_ASSERT_X(data.size() == 512, "ArtNetSubnetSender::sendUniverse", "data has not length 512");

    QVector<uint8_t>& universePacket = m_preparedPackets[universe];

    // fill raw DMX data in prepared packet:
    std::copy(data.begin(), data.end(), universePacket.begin() + 18);
    // set sequence number:
    universePacket[12] = m_sequenceNumber;
    m_sequenceNumber = (m_sequenceNumber + 1) % 256;

    // send prepared packet with filled in data via UDP socket:
    const char* packet = reinterpret_cast<const char*>(universePacket.constData());
    m_udpSocket.writeDatagram(packet, 18+512, QHostAddress::Broadcast, ARTNET_PORT);
}

void ArtNetSubnetSender::sendUniverseUnicast(int universe, const QVector<uint8_t>& data, const QVector<QHostAddress>& addresses) {
    Q_ASSERT_X(universe <= 15, "ArtNetSubnetSender::sendUniverse", "Universe number too large");
    Q_ASSERT_X(universe >= 0, "ArtNetSubnetSender::sendUniverse", "Universe number too small");
    Q_ASSERT_X(data.size() == 512, "ArtNetSubnetSender::sendUniverse", "data has not length 512");

    QVector<uint8_t>& universePacket = m_preparedPackets[universe];

    // fill raw DMX data in prepared packet:
    std::copy(data.begin(), data.end(), universePacket.begin() + 18);
    // set sequence number:
    universePacket[12] = m_sequenceNumber;
    m_sequenceNumber = (m_sequenceNumber + 1) % 256;

    // send prepared packet with filled in data via UDP socket:
    const char* packet = reinterpret_cast<const char*>(universePacket.constData());

    for (const QHostAddress& address: addresses) {
        m_udpSocket.writeDatagram(packet, 18+512, address, ARTNET_PORT);
    }
}

void ArtNetSubnetSender::preparePackets(int net, int subnet) {
    net = limit(0, net, 127);
    subnet = limit(0, subnet, 15);

    // each subnet has 16 universes:
    m_preparedPackets.resize(16);

    // iterate over all these universes:
    for (int universe = 0; universe < 16; ++universe) {
        QVector<uint8_t>& universePacket = m_preparedPackets[universe];
        // the packet for each universe has a length of 18 + 512 bytes:
        universePacket.resize(530);

        uint16_t address = uint16_t(net << 8 | subnet << 4 | universe);

        // packet starts with specific string:
        universePacket[0] = 'A';
        universePacket[1] = 'r';
        universePacket[2] = 't';
        universePacket[3] = '-';
        universePacket[4] = 'N';
        universePacket[5] = 'e';
        universePacket[6] = 't';
        universePacket[7] = 0;
        // OPCODE for ArtDMX packet
        universePacket[8] = 0x00; // OpCode Low Byte as per spec
        universePacket[9] = 0x50; // OpCode High Byte as per spec
        // VERSION
        universePacket[10] = 0; // ver High
        universePacket[11] = 14; // ver Low
        // universePacket[12] is sequence number
        // PHYSICAL PORT
        universePacket[13] = 1;
        // ADDRESS
        universePacket[14] = lowByte(address); // address low byte first
        universePacket[15] = highByte(address); // address high byte
        // DMX LENGTH
        universePacket[16] = highByte(512); // HI Byte
        universePacket[17] = lowByte(512); // LOW Byte

        // the rest of the message (18-530) is the raw DMX data and is filled later
    }
}
