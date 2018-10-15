#include "BasicSAcnSender.h"

#include <QtNetwork>
#include <algorithm>

#include "utils.h"


const int SACN_PORT = 5568;

inline uint8_t lowByte(uint16_t value) {
    return uint8_t(value & 0xff);
}

inline uint8_t highByte(uint16_t value) {
    return uint8_t(value >> 8);
}

inline QByteArray intTo16Bit(int i) {
    QByteArray a;
    a.append(char((i >> 8) & 0xff));
    a.append(char(i & 0xff));
    return a;
}

inline QByteArray lengthAsLow12(int i) {
    return intTo16Bit(0x7000 | i);
}


SAcnSender::SAcnSender(int startUniverse, int priority)
    : m_sequenceNumber(0)
    , m_startUniverse(startUniverse)
    , m_priority(priority)
    , m_uuid(QUuid::createUuid())
{
    preparePackets();
}

void SAcnSender::setStartUniverse(int startUniverse) {
    m_startUniverse = limit(1, startUniverse, 63999 - 16);
    preparePackets();
}

void SAcnSender::setPriority(int priority) {
    m_priority = limit(1, priority, 200);
    preparePackets();
}

void SAcnSender::sendUniverseMulticast(int universe, const QVector<uint8_t>& data) {
    Q_ASSERT_X(universe <= 15, "SAcnSender::sendUniverse", "Universe number too large");
    Q_ASSERT_X(universe >= 0, "SAcnSender::sendUniverse", "Universe number too small");
    Q_ASSERT_X(data.size() == 512, "SAcnSender::sendUniverse", "data has not length 512");

    QVector<uint8_t>& universePacket = m_preparedPackets[universe];

    // fill raw DMX data in prepared packet:
    std::copy(data.begin(), data.end(), universePacket.end() - 512);
    // set sequence number:
    universePacket[111] = m_sequenceNumber;
    m_sequenceNumber = (m_sequenceNumber + 1) % 256;

    // send prepared packet with filled in data via UDP socket:
    const char* packet = reinterpret_cast<const char*>(universePacket.constData());

    m_udpSocket.writeDatagram(packet, universePacket.size(), m_multicastAddresses[universe], SACN_PORT);
}

void SAcnSender::sendUniverseUnicast(int universe, const QVector<uint8_t>& data, const QVector<QHostAddress>& addresses) {
    Q_ASSERT_X(universe <= 15, "SAcnSender::sendUniverse", "Universe number too large");
    Q_ASSERT_X(universe >= 0, "SAcnSender::sendUniverse", "Universe number too small");
    Q_ASSERT_X(data.size() == 512, "SAcnSender::sendUniverse", "data has not length 512");

    QVector<uint8_t>& universePacket = m_preparedPackets[universe];

    // fill raw DMX data in prepared packet:
    std::copy(data.begin(), data.end(), universePacket.end() - 512);
    // set sequence number:
    universePacket[111] = m_sequenceNumber;
    m_sequenceNumber = (m_sequenceNumber + 1) % 256;

    // send prepared packet with filled in data via UDP socket:
    const char* packet = reinterpret_cast<const char*>(universePacket.constData());

    for (const QHostAddress& address: addresses) {
        m_udpSocket.writeDatagram(packet, universePacket.size(), address, SACN_PORT);
    }
}

void SAcnSender::preparePackets() {
    // Luminosus is limited to 16 consecutive universes:
    m_preparedPackets.resize(16);
    m_multicastAddresses.resize(16);

    const int dataLengthPerPacket = 512;

    // iterate over all these universes:
    for (int universe = 0; universe < 16; ++universe) {
        QVector<uint8_t>& universePacket = m_preparedPackets[universe];

        // DMP Layer:
        QByteArray dmpLayer;
        // packet length
        dmpLayer.append(lengthAsLow12(10 + 1 + dataLengthPerPacket));
        // vector
        dmpLayer.append(0x02);
        // address type and data type
        dmpLayer.append(0xa1);
        // start code
        dmpLayer.append("\x00\x00", 2);
        // increment value
        dmpLayer.append("\x00\x01", 2);
        // value count
        dmpLayer.append(intTo16Bit(1 + dataLengthPerPacket));
        // DMX 512 start code
        dmpLayer.append(char(0x00));
        // DMX 512 data
        // filled in later...
        dmpLayer.append(dataLengthPerPacket, char(0x00));

        // Framing Layer:
        QByteArray framingLayer;
        // packet length
        framingLayer.append(lengthAsLow12(77 + dmpLayer.size()));
        // vector
        framingLayer.append("\x00\x00\x00\x02", 4);
        // name (64 bytes)
        QByteArray name = QString("Luminosus").toLatin1();
        framingLayer.append(name);
        framingLayer.append(64 - name.size(), 0x00);
        // priority
        framingLayer.append(int8_t(m_priority));
        // reserved by spec
        framingLayer.append("\x00\x00", 2);
        // sequence
        framingLayer.append(char(0x01));
        // options
        framingLayer.append(char(0x00));
        // universe
        framingLayer.append(intTo16Bit(m_startUniverse + universe));
        framingLayer.append(dmpLayer);


        // Root Layer:
        QByteArray rootLayer;
        rootLayer.append("\x00\x10\x00\x00", 4);
        rootLayer.append("ASC-E1.17\x00\x00\x00", 12);
        // pdu size starts after byte 16 - there are 38 bytes of data in root layer
        // so size is 38 - 16 + framing layer
        rootLayer.append(lengthAsLow12(38 - 16 + framingLayer.size()));
        rootLayer.append("\x00\x00\x00\x04", 4);
        rootLayer.append(m_uuid.toRfc4122());
        rootLayer.append(framingLayer);

        if (rootLayer.size() != 638) {
            qCritical() << "sACN packet has wrong size, maybe a data type problem";
            qCritical() << "Packet size:" << rootLayer.size();
        }

        universePacket.resize(rootLayer.size());
        std::copy(rootLayer.begin(), rootLayer.end(), universePacket.begin());

        // Multicast IP for this universe:
        QString ip = QString("239.255.%1.%2").arg(highByte(m_startUniverse + universe)).arg(lowByte(m_startUniverse + universe));
        QHostAddress addr(ip);
        m_multicastAddresses[universe] = addr;
    }
}
