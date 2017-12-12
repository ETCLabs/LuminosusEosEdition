#include <gtest/gtest.h>

#include "../src/Matrix.h"

TEST(Matrix, InitialState) {
    ColorMatrix m;

    ASSERT_EQ(m.getSX(), 1);
    ASSERT_EQ(m.getSY(), 1);
    ASSERT_EQ(m.getSize(), QSize(1, 1));
    ASSERT_EQ(m.getHsv().size(), 1);
    ASSERT_EQ(m.getHsv().at(0).size(), 1);
    ASSERT_EQ(m.getHsv()[0][0], colorVector(3, 0.0));
    ASSERT_EQ(m.getHsvAt(0, 0), colorVector(3, 0.0));
    ASSERT_EQ(m.getRgb().size(), 1);
    ASSERT_EQ(m.getRgb().at(0).size(), 1);
    ASSERT_EQ(m.getRgb()[0][0], colorVector(3, 0.0));
    ASSERT_EQ(m.getRgbAt(0, 0), colorVector(3, 0.0));
    ASSERT_EQ(m.getValue(), 0.0);
    ASSERT_EQ(m.getAbsoluteValue(), 0.0);
    ASSERT_EQ(m.absoluteMaximumIsProvided(), false);
}

TEST(Matrix, ValidRescale) {
    ColorMatrix m;

    m.rescaleTo(2, 3);
    ASSERT_EQ(m.getSX(), 2);
    ASSERT_EQ(m.getSY(), 3);
    ASSERT_EQ(m.getSize(), QSize(2, 3));
    ASSERT_EQ(m.getHsv().size(), 2);
    ASSERT_EQ(m.getHsv().at(0).size(), 3);
    ASSERT_EQ(m.getRgb().size(), 2);
    ASSERT_EQ(m.getRgb().at(0).size(), 3);

    // Matrix is atm too slow for 1080p and higher
    m.rescaleTo(200, 200);
    ASSERT_EQ(m.getSX(), 200);
    ASSERT_EQ(m.getSY(), 200);
    ASSERT_EQ(m.getSize(), QSize(200, 200));
    ASSERT_EQ(m.getHsv().size(), 200);
    ASSERT_EQ(m.getHsv().at(0).size(), 200);
    ASSERT_EQ(m.getRgb().size(), 200);
    ASSERT_EQ(m.getRgb().at(0).size(), 200);
}

TEST(Matrix, InvalidRescale) {
    ColorMatrix m;
    m.rescaleTo(0, 0);

    ASSERT_EQ(m.getSX(), 1);
    ASSERT_EQ(m.getSY(), 1);
    ASSERT_EQ(m.getSize(), QSize(1, 1));
    ASSERT_EQ(m.getHsv().size(), 1);
    ASSERT_EQ(m.getHsv().at(0).size(), 1);
    ASSERT_EQ(m.getRgb().size(), 1);
    ASSERT_EQ(m.getRgb().at(0).size(), 1);
}

TEST(Matrix, setHsv) {
    ColorMatrix m;
    m.rescaleTo(2, 2);
    colorVector expectedColor{0.5, 0.6, 0.7};

    // set all elements to same color:
    m.setHsv(0.5, 0.6, 0.7);

    ASSERT_EQ(m.getHsv()[0][0], expectedColor);
    ASSERT_EQ(m.getHsv()[1][0], expectedColor);
    ASSERT_EQ(m.getHsv()[0][1], expectedColor);
    ASSERT_EQ(m.getHsv()[1][1], expectedColor);
    ASSERT_EQ(m.getHsvAt(0, 0), expectedColor);
    ASSERT_EQ(m.getHsvAt(1, 0), expectedColor);
    ASSERT_EQ(m.getHsvAt(0, 1), expectedColor);
    ASSERT_EQ(m.getHsvAt(1, 1), expectedColor);

    // set elements to color of another 2D vector:
    m.setHsv(colorData2d(2, std::vector<colorVector>(2, expectedColor)));
    ASSERT_EQ(m.getHsv()[0][0], expectedColor);
    ASSERT_EQ(m.getHsv()[1][0], expectedColor);
    ASSERT_EQ(m.getHsv()[0][1], expectedColor);
    ASSERT_EQ(m.getHsv()[1][1], expectedColor);
    ASSERT_EQ(m.getHsvAt(0, 0), expectedColor);
    ASSERT_EQ(m.getHsvAt(1, 0), expectedColor);
    ASSERT_EQ(m.getHsvAt(0, 1), expectedColor);
    ASSERT_EQ(m.getHsvAt(1, 1), expectedColor);

    colorVector secondColor{0.2, 0.3, 0.4};
    m.setHsvAt(0, 1, 0.2, 0.3, 0.4);
    ASSERT_EQ(m.getHsvAt(0, 0), expectedColor);
    ASSERT_EQ(m.getHsvAt(0, 1), secondColor);
}

TEST(Matrix, setRgb) {
    ColorMatrix m;
    m.rescaleTo(2, 2);
    colorVector expectedColor{0.5, 0.6, 0.7};

    // set all elements to same color:
    m.setRgb(0.5, 0.6, 0.7);

    ASSERT_EQ(m.getRgb()[0][0], expectedColor);
    ASSERT_EQ(m.getRgb()[1][0], expectedColor);
    ASSERT_EQ(m.getRgb()[0][1], expectedColor);
    ASSERT_EQ(m.getRgb()[1][1], expectedColor);
    ASSERT_EQ(m.getRgbAt(0, 0), expectedColor);
    ASSERT_EQ(m.getRgbAt(1, 0), expectedColor);
    ASSERT_EQ(m.getRgbAt(0, 1), expectedColor);
    ASSERT_EQ(m.getRgbAt(1, 1), expectedColor);

    // set elements to color of another 2D vector:
    m.setRgb(colorData2d(2, std::vector<colorVector>(2, expectedColor)));
    ASSERT_EQ(m.getRgb()[0][0], expectedColor);
    ASSERT_EQ(m.getRgb()[1][0], expectedColor);
    ASSERT_EQ(m.getRgb()[0][1], expectedColor);
    ASSERT_EQ(m.getRgb()[1][1], expectedColor);
    ASSERT_EQ(m.getRgbAt(0, 0), expectedColor);
    ASSERT_EQ(m.getRgbAt(1, 0), expectedColor);
    ASSERT_EQ(m.getRgbAt(0, 1), expectedColor);
    ASSERT_EQ(m.getRgbAt(1, 1), expectedColor);

    colorVector secondColor{0.2, 0.3, 0.4};
    m.setRgbAt(0, 1, 0.2, 0.3, 0.4);
    ASSERT_EQ(m.getRgbAt(0, 0), expectedColor);
    ASSERT_EQ(m.getRgbAt(0, 1), secondColor);
}

TEST(Matrix, setValue) {
    ColorMatrix m;
    ASSERT_EQ(m.getValue(), 0.0);

    m.setValue(0.2);
    ASSERT_EQ(m.getValue(), 0.2);

    m.setValue(1.0);
    ASSERT_EQ(m.getValue(), 1.0);

    m.setValue(0.0);
    ASSERT_EQ(m.getValue(), 0.0);

    m.setValue(1.5);
    EXPECT_EQ(m.getValue(), 1.0);

    m.setValue(-1.0);
    EXPECT_EQ(m.getValue(), 0.0);
}


TEST(Matrix, setAbsoluteValue) {
    ColorMatrix m;
    ASSERT_EQ(m.getAbsoluteValue(), 0.0);

    m.setAbsoluteValue(0.2);
    ASSERT_EQ(m.getAbsoluteValue(), 0.2);
}
