#include <gtest/gtest.h>

#include "../Matrix.h"

TEST(Matrix, Init) {
    ColorMatrix m;

    ASSERT_EQ(m.getSX(), 1);
    ASSERT_EQ(m.getSY(), 1);
}
