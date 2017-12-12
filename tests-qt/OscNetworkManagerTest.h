#ifndef OSCNETWORKMANAGERTEST_H
#define OSCNETWORKMANAGERTEST_H

#include <QtTest>

#include "../src/OSCNetworkManager.h"

// http://www.rfc-editor.org/rfc/rfc1055.txt
#define SLIP_END		0xc0    /* indicates end of packet */
#define SLIP_ESC		0xdb    /* indicates byte stuffing */
#define SLIP_ESC_END	0xdc    /* ESC ESC_END means END data byte */
#define SLIP_ESC_ESC	0xdd    /* ESC ESC_ESC means ESC data byte */

#define SLIP_CHAR(x)	static_cast<char>(static_cast<unsigned char>(x))


class OSCNetworkManagerExposed : public OSCNetworkManager {
public:
    using OSCNetworkManager::popSlipFramedPacketFromStreamData;
};


class OscNetworkManagerTest : public QObject {

    Q_OBJECT

private slots:
    // functions executed by QtTest before and after test suite
    void initTestCase() {}
    void cleanupTestCase() {}

    // functions executed by QtTest before and after each test
    void init() {}
    void cleanup() {}

    // test functions - all functions prefixed with "test" will be ran as tests
    void testInitialState() {
        OSCNetworkManager osc;

        QCOMPARE(osc.isConnected(), false);
        // TODO: check other state
    }

    void testNormalSlipPacket() {
        OSCNetworkManagerExposed osc;

        QByteArray data("test", 4);
        QByteArray slipFramedPacket;
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));
        slipFramedPacket.append(data);
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));

        QByteArray result = osc.popSlipFramedPacketFromStreamData(slipFramedPacket);
        QCOMPARE(result, data);
        QCOMPARE(slipFramedPacket.size(), 0);
    }

    void testExtendedSlipPacket() {
        OSCNetworkManagerExposed osc;

        QByteArray data("test", 4);
        QByteArray nextPacket("nextPacket", 10);
        QByteArray slipFramedPacket;
        slipFramedPacket.append("someOtherData");
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));
        slipFramedPacket.append(data);
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));
        slipFramedPacket.append(nextPacket);

        QByteArray result = osc.popSlipFramedPacketFromStreamData(slipFramedPacket);
        QCOMPARE(result, data);
        QCOMPARE(slipFramedPacket, nextPacket);
    }

    void testIncompleteSlipPacket() {
        OSCNetworkManagerExposed osc;

        QByteArray data("test", 4);
        QByteArray slipFramedPacket;
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));
        slipFramedPacket.append(data);
        // no SLIP END character here -> incomplete!
        QByteArray slipFramedPacketCopy(slipFramedPacket);

        QByteArray result = osc.popSlipFramedPacketFromStreamData(slipFramedPacket);
        QCOMPARE(result, QByteArray());
        QCOMPARE(slipFramedPacket, slipFramedPacketCopy);
    }

    void testGarbageSlipPacket() {
        OSCNetworkManagerExposed osc;

        QByteArray slipFramedPacket("garbage");
        // no SLIP END character here -> garbage!

        QByteArray result = osc.popSlipFramedPacketFromStreamData(slipFramedPacket);
        QCOMPARE(result, QByteArray());
        QCOMPARE(slipFramedPacket.size(), 0);
    }

    void testSlipPacketEdgeCases() {
        OSCNetworkManagerExposed osc;

        QByteArray emptyPacket;
        QByteArray result = osc.popSlipFramedPacketFromStreamData(emptyPacket);
        QCOMPARE(result, QByteArray());
        QCOMPARE(emptyPacket.size(), 0);

        // large packet, 50MB
        QByteArray data(50*1024*1024, 0x0);
        QByteArray slipFramedPacket;
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));
        slipFramedPacket.append(data);
        slipFramedPacket.append(SLIP_CHAR(SLIP_END));

        result = osc.popSlipFramedPacketFromStreamData(slipFramedPacket);
        QCOMPARE(result, data);
        QCOMPARE(slipFramedPacket.size(), 0);
    }

};

#endif // OSCNETWORKMANAGERTEST_H
