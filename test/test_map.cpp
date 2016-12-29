#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

namespace {
    using result_type = opex::result<int>;

    result_type my_opex_enabled_function(bool fail) {
        if (fail)
            return result_type::make_exception<std::runtime_error>("fail!");

        static int id{0};
        return result_type{++id};
    }
}

TEST(Map, ValidResult)
{
    auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(result1.unwrap() + 2, result2.unwrap());
}

TEST(Map, ValidConstResult)
{
    const auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(result1.unwrap() + 2, result2.unwrap());
}

TEST(Map, ValidRvalueResult)
{
    auto result1 = my_opex_enabled_function(false);
    EXPECT_TRUE(result1.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    const auto value = result1.unwrap();

    const auto result2 = std::move(result1).map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(value + 2, result2.unwrap());
}

TEST(Map, InvalidResult)
{
    auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
}

TEST(Map, InvalidConstResult)
{
    const auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
}

TEST(Map, InvalidRvalueResult)
{
    auto result1 = my_opex_enabled_function(true);
    EXPECT_TRUE(result1.is_err());

    const auto result2 = std::move(result1).map([](int i) {
        return i + 2;
    });
    EXPECT_TRUE(result2.is_err());
}
