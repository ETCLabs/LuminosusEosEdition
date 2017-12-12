#ifndef OSCNETWORKMANAGERTEST_H
#define OSCNETWORKMANAGERTEST_H

#include <QtTest>

#include "../src/OSCNetworkManager.h"


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
    }

};

#endif // OSCNETWORKMANAGERTEST_H
