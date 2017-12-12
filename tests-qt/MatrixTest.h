#ifndef MATRIXTEST_H
#define MATRIXTEST_H

#include <QtTest>

#include "../src/Matrix.h"


class MatrixTest : public QObject {

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
        ColorMatrix m;

        QCOMPARE(m.getSX(), 1);
        QCOMPARE(m.getSY(), 1);
        QCOMPARE(m.getSize(), QSize(1, 1));
        QCOMPARE(m.getHsv().size(), 1);
        QCOMPARE(m.getHsv().at(0).size(), 1);
        QCOMPARE(m.getHsv()[0][0], colorVector(3, 0.0));
        QCOMPARE(m.getHsvAt(0, 0), colorVector(3, 0.0));
        QCOMPARE(m.getRgb().size(), 1);
        QCOMPARE(m.getRgb().at(0).size(), 1);
        QCOMPARE(m.getRgb()[0][0], colorVector(3, 0.0));
        QCOMPARE(m.getRgbAt(0, 0), colorVector(3, 0.0));
        QCOMPARE(m.getValue(), 0.0);
        QCOMPARE(m.getAbsoluteValue(), 0.0);
        QCOMPARE(m.absoluteMaximumIsProvided(), false);
    }

    void testValidRescale() {
        ColorMatrix m;

        m.rescaleTo(2, 3);
        QCOMPARE(m.getSX(), 2);
        QCOMPARE(m.getSY(), 3);
        QCOMPARE(m.getSize(), QSize(2, 3));
        QCOMPARE(m.getHsv().size(), 2);
        QCOMPARE(m.getHsv().at(0).size(), 3);
        QCOMPARE(m.getRgb().size(), 2);
        QCOMPARE(m.getRgb().at(0).size(), 3);

        // Matrix is atm too slow for 1080p and higher
        m.rescaleTo(200, 200);
        QCOMPARE(m.getSX(), 200);
        QCOMPARE(m.getSY(), 200);
        QCOMPARE(m.getSize(), QSize(200, 200));
        QCOMPARE(m.getHsv().size(), 200);
        QCOMPARE(m.getHsv().at(0).size(), 200);
        QCOMPARE(m.getRgb().size(), 200);
        QCOMPARE(m.getRgb().at(0).size(), 200);

        QBENCHMARK {
            m.rescaleTo(1, 1);
            m.rescaleTo(200, 200);
        }
    }

    void testInvalidRescale() {
        ColorMatrix m;
        m.rescaleTo(0, 0);

        QCOMPARE(m.getSX(), 1);
        QCOMPARE(m.getSY(), 1);
        QCOMPARE(m.getSize(), QSize(1, 1));
        QCOMPARE(m.getHsv().size(), 1);
        QCOMPARE(m.getHsv().at(0).size(), 1);
        QCOMPARE(m.getRgb().size(), 1);
        QCOMPARE(m.getRgb().at(0).size(), 1);
    }

    void testSetHsv() {
        ColorMatrix m;
        m.rescaleTo(2, 2);
        colorVector expectedColor{0.5, 0.6, 0.7};

        // set all elements to same color:
        m.setHsv(0.5, 0.6, 0.7);

        QCOMPARE(m.getHsv()[0][0], expectedColor);
        QCOMPARE(m.getHsv()[1][0], expectedColor);
        QCOMPARE(m.getHsv()[0][1], expectedColor);
        QCOMPARE(m.getHsv()[1][1], expectedColor);
        QCOMPARE(m.getHsvAt(0, 0), expectedColor);
        QCOMPARE(m.getHsvAt(1, 0), expectedColor);
        QCOMPARE(m.getHsvAt(0, 1), expectedColor);
        QCOMPARE(m.getHsvAt(1, 1), expectedColor);

        // set elements to color of another 2D vector:
        m.setHsv(colorData2d(2, std::vector<colorVector>(2, expectedColor)));
        QCOMPARE(m.getHsv()[0][0], expectedColor);
        QCOMPARE(m.getHsv()[1][0], expectedColor);
        QCOMPARE(m.getHsv()[0][1], expectedColor);
        QCOMPARE(m.getHsv()[1][1], expectedColor);
        QCOMPARE(m.getHsvAt(0, 0), expectedColor);
        QCOMPARE(m.getHsvAt(1, 0), expectedColor);
        QCOMPARE(m.getHsvAt(0, 1), expectedColor);
        QCOMPARE(m.getHsvAt(1, 1), expectedColor);

        colorVector secondColor{0.2, 0.3, 0.4};
        m.setHsvAt(0, 1, 0.2, 0.3, 0.4);
        QCOMPARE(m.getHsvAt(0, 0), expectedColor);
        QCOMPARE(m.getHsvAt(0, 1), secondColor);

        m.rescaleTo(200, 200);
        QBENCHMARK {
            // set all elements to same color:
            m.setHsv(0.5, 0.6, 0.7);
        }
    }

    void testSetRgb() {
        ColorMatrix m;
        m.rescaleTo(2, 2);
        colorVector expectedColor{0.5, 0.6, 0.7};

        // set all elements to same color:
        m.setRgb(0.5, 0.6, 0.7);

        QCOMPARE(m.getRgb()[0][0], expectedColor);
        QCOMPARE(m.getRgb()[1][0], expectedColor);
        QCOMPARE(m.getRgb()[0][1], expectedColor);
        QCOMPARE(m.getRgb()[1][1], expectedColor);
        QCOMPARE(m.getRgbAt(0, 0), expectedColor);
        QCOMPARE(m.getRgbAt(1, 0), expectedColor);
        QCOMPARE(m.getRgbAt(0, 1), expectedColor);
        QCOMPARE(m.getRgbAt(1, 1), expectedColor);

        // set elements to color of another 2D vector:
        m.setRgb(colorData2d(2, std::vector<colorVector>(2, expectedColor)));
        QCOMPARE(m.getRgb()[0][0], expectedColor);
        QCOMPARE(m.getRgb()[1][0], expectedColor);
        QCOMPARE(m.getRgb()[0][1], expectedColor);
        QCOMPARE(m.getRgb()[1][1], expectedColor);
        QCOMPARE(m.getRgbAt(0, 0), expectedColor);
        QCOMPARE(m.getRgbAt(1, 0), expectedColor);
        QCOMPARE(m.getRgbAt(0, 1), expectedColor);
        QCOMPARE(m.getRgbAt(1, 1), expectedColor);

        colorVector secondColor{0.2, 0.3, 0.4};
        m.setRgbAt(0, 1, 0.2, 0.3, 0.4);
        QCOMPARE(m.getRgbAt(0, 0), expectedColor);
        QCOMPARE(m.getRgbAt(0, 1), secondColor);

        m.rescaleTo(200, 200);
        QBENCHMARK {
            // set all elements to same color:
            m.setRgb(0.5, 0.6, 0.7);
        }
    }

    void testSetValue() {
        ColorMatrix m;
        QCOMPARE(m.getValue(), 0.0);

        m.setValue(0.2);
        QCOMPARE(m.getValue(), 0.2);

        m.setValue(1.0);
        QCOMPARE(m.getValue(), 1.0);

        m.setValue(0.0);
        QCOMPARE(m.getValue(), 0.0);

        m.setValue(1.5);
        QCOMPARE(m.getValue(), 1.0);

        m.setValue(-1.0);
        QCOMPARE(m.getValue(), 0.0);
    }

    void testSetAbsoluteValue() {
        ColorMatrix m;
        QCOMPARE(m.getAbsoluteValue(), 0.0);

        m.setAbsoluteValue(0.2);
        QCOMPARE(m.getAbsoluteValue(), 0.2);
    }
};


#endif // MATRIXTEST_H
