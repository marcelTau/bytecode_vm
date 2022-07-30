#include <gtest/gtest.h>

#include "header.h"

TEST(TODO_WriteTests, TODO_WriteTestsDescription) {
    EXPECT_TRUE(add_2(5) == 7);
}

TEST(ActuallyDoSomething, xxx) {
    EXPECT_TRUE(branches(5) == 5);
    EXPECT_TRUE(branches(8) == 0);
    EXPECT_TRUE(branches(2) == 2);
    EXPECT_TRUE(branches(1) == 1);
    EXPECT_TRUE(branches(3) == 3);
    EXPECT_TRUE(branches(4) == 4);
}
