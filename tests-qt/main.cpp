#include <QtTest>
#include "MatrixTest.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MatrixTest matrixTest;

    return QTest::qExec(&matrixTest, argc, argv);
}
