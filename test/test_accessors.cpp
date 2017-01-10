#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

TEST(Accessors, ValidValue) {
    gear::TestType value;
    gear::TestResult result{value};

    EXPECT_EQ(value, result.unwrap());
    EXPECT_EQ(value, *result);
    EXPECT_EQ(value.id(), result->id());
}

TEST(Accessors, ValidConstValue) {
    gear::TestType value;
    const gear::TestResult result{value};

    EXPECT_EQ(value, result.unwrap());
    EXPECT_EQ(value, *result);
    EXPECT_EQ(value.id(), result->id());
}

TEST(Accessors, ValidRValue) {
    gear::TestType value;
    gear::TestResult result{value};

    EXPECT_EQ(value, std::move(result).unwrap());
    EXPECT_EQ(value, *std::move(result));
    EXPECT_EQ(value.id(), std::move(result)->id());
}

TEST(Accessors, InvalidValue) {
    auto result = gear::TestResult::make_exception<gear::TestException>("");

    EXPECT_THROW(result.unwrap(), gear::TestException);
    EXPECT_THROW(*result, gear::TestException);
    EXPECT_THROW(result->id(), gear::TestException);
}

TEST(Accessors, InvalidConstValue) {
    const auto result = gear::TestResult::make_exception<gear::TestException>("");

    EXPECT_THROW(result.unwrap(), gear::TestException);
    EXPECT_THROW(*result, gear::TestException);
    EXPECT_THROW(result->id(), gear::TestException);
}
