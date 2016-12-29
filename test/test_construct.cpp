#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

TEST(Construct, Lvalue)
{
    gear::TestType value;
    const auto result = gear::TestResult{value};

    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_err());
    EXPECT_EQ(value, result.unwrap());
    EXPECT_TRUE(value.valid());
    EXPECT_TRUE(result.unwrap().valid());
}

TEST(Construct, Rvalue)
{
    gear::TestType value;
    const auto result = gear::TestResult{std::move(value)};

    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_err());
    EXPECT_NE(value, result.unwrap());
    EXPECT_FALSE(value.valid());
    EXPECT_TRUE(result.unwrap().valid());
}

TEST(Construct, Move)
{
    gear::TestType value;
    auto result1 = gear::TestResult{value};
    const auto result2 = gear::TestResult{std::move(result1)};

    EXPECT_TRUE(result2.is_ok());
    EXPECT_FALSE(result2.is_err());
    EXPECT_EQ(value, result2.unwrap());
    EXPECT_TRUE(value.valid());
    EXPECT_TRUE(result2.unwrap().valid());
}

TEST(Construct, FromException)
{
    const auto result = gear::TestResult::from_exception(gear::TestException{"FromException"});

    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_err());
    EXPECT_THROW(result.unwrap(), gear::TestException);
}

TEST(Construct, MakeException)
{
    const auto result = gear::TestResult::make_exception<gear::TestException>("MakeException");

    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_err());
    EXPECT_THROW(result.unwrap(), gear::TestException);
}

TEST(Construct, Call)
{
    const auto result = gear::TestResult::call(std::bind(gear::throw_if_true, false));

    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_err());
    EXPECT_NO_THROW(result.unwrap());
    EXPECT_TRUE(result.unwrap().valid());
}

TEST(Construct, CallExcept)
{
    const auto result = gear::TestResult::call(std::bind(gear::throw_if_true, true));

    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_err());
    EXPECT_THROW(result.unwrap(), gear::TestException);
}
