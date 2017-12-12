#include <QtTest>

#include "MatrixTest.h"
#include "OscNetworkManagerTest.h"
#include "BlockCreationTest.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QList<QObject*> testCases;
    testCases << new MatrixTest();
    testCases << new OscNetworkManagerTest();
    testCases << new BlockCreationTest();

    int returnValue = 0;
    for (QObject* testCase: testCases) {
        returnValue |= QTest::qExec(testCase, argc, argv);
    }
    return returnValue;
}
