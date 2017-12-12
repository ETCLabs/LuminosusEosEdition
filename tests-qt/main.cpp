#include <QtTest>

#include "MatrixTest.h"
#include "OscNetworkManagerTest.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QList<QObject*> testCases;
    testCases << new MatrixTest();
    testCases << new OscNetworkManagerTest();

    int returnValue = 0;
    for (QObject* testCase: testCases) {
        returnValue |= QTest::qExec(testCase, argc, argv);
    }
    return returnValue;
}
