#ifndef BASICSACNSENDER_H
#define BASICSACNSENDER_H

#include <cstdint>
#include <QUdpSocket>
#include <QVector>
#include <QObject>
#include <QUuid>


class SAcnSender
{

public:
    SAcnSender(int startUniverse = 1, int priority = 100);

    void setStartUniverse(int startUniverse);

    void setPriority(int priority);

    void sendUniverseMulticast(int universe, const QVector<uint8_t>& data);

    void sendUniverseUnicast(int universe, const QVector<uint8_t>& data, const QVector<QHostAddress>& addresses);

private:
    void preparePackets();

protected:
    uint8_t m_sequenceNumber;
    QVector<QVector<uint8_t>> m_preparedPackets;
    QVector<QHostAddress> m_multicastAddresses;
    QUdpSocket m_udpSocket;
    int m_startUniverse;
    int m_priority;
    QUuid m_uuid;
};

#endif // BASICSACNSENDER_H
