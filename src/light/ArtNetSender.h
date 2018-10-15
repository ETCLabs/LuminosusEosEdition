#ifndef ARTNETSENDER_H
#define ARTNETSENDER_H

#include <cstdint>
#include <QUdpSocket>
#include <QVector>
#include <QObject>


class ArtNetSubnetSender
{

public:
    ArtNetSubnetSender(int net, int subnet);

    void setNetAndSubnet(int net, int subnet);

    void sendUniverseBroadcast(int universe, const QVector<uint8_t>& data);

    void sendUniverseUnicast(int universe, const QVector<uint8_t>& data, const QVector<QHostAddress>& addresses);

private:
    void preparePackets(int net, int subnet);

protected:
    uint8_t m_sequenceNumber;
    QVector<QVector<uint8_t>> m_preparedPackets;
    QUdpSocket m_udpSocket;
};

#endif // ARTNETSENDER_H

