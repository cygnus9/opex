#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

TEST(Call, Value)
{
    const opex::result<gear::TestType> result = opex::call(std::bind(gear::throw_if_true, false));

    EXPECT_TRUE(result.is_ok());
    ASSERT_NO_THROW(result.unwrap());
    EXPECT_TRUE(result.unwrap().valid());
}

TEST(Call, ValueExplicitExceptionType)
{
    const gear::TestResult result = opex::call<gear::TestException>(std::bind(gear::throw_if_true, false));

    EXPECT_TRUE(result.is_ok());
    ASSERT_NO_THROW(result.unwrap());
    EXPECT_TRUE(result.unwrap().valid());
}

TEST(Call, Except)
{
    const opex::result<gear::TestType> result = opex::call(std::bind(gear::throw_if_true, true));

    EXPECT_TRUE(result.is_err());
    EXPECT_THROW(result.unwrap(), gear::TestException);
}

TEST(Call, ExceptExplicitExceptionType)
{
    const gear::TestResult result = opex::call<gear::TestException>(std::bind(gear::throw_if_true, true));

    EXPECT_TRUE(result.is_err());
    EXPECT_THROW(result.unwrap(), gear::TestException);
}
