#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

namespace {
    using result_type = opex::result<gear::TestType, gear::TestException>;

    result_type my_opex_enabled_function(bool fail) {
        if (fail)
            return result_type::make_exception<gear::TestException>("fail!");

        return result_type{gear::TestType{}};
    }
    
    struct OtherType {
        gear::TestType nested;
    };
    using other_result_type = opex::result<OtherType, std::exception>;
    
    template <typename T>
    other_result_type nest_value(T &&v) {
        return other_result_type{OtherType{std::forward<T>(v)}};
    }
}

TEST(AndThen, ValidResult)
{
    auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.and_then([](const gear::TestType &value) {
        return nest_value(value);
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(result1.unwrap(), result2.unwrap().nested);
}

TEST(AndThen, ValidConstResult)
{
    const auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.and_then([](const gear::TestType &value) {
        return nest_value(value);
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(result1.unwrap(), result2.unwrap().nested);
}

TEST(AndThen, ValidRvalueResult)
{
    auto result1 = my_opex_enabled_function(false);
    EXPECT_TRUE(result1.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    const auto value = result1.unwrap();

    const auto result2 = std::move(result1).and_then([](gear::TestType &&value) {
        return nest_value(std::move(value));
    });
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(value, result2.unwrap().nested);
}

TEST(AndThen, InvalidResult)
{
    auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.and_then([](const gear::TestType &value) {
        return nest_value(value);
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(result1.what(), result2.what());
}

TEST(AndThen, InvalidConstResult)
{
    const auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.and_then([](const gear::TestType &value) {
        return nest_value(value);
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(result1.what(), result2.what());
}

TEST(AndThen, InvalidRvalueResult)
{
    auto result1 = my_opex_enabled_function(true);
    EXPECT_TRUE(result1.is_err());
    const auto what = result1.what();

    const auto result2 = std::move(result1).and_then([](gear::TestType &&value) {
        return nest_value(std::move(value));
    });
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(what, result2.what());
}
