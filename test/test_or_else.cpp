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

    struct OtherException : std::exception {
        gear::TestException nested;

        OtherException(gear::TestException &&exc): nested(std::move(exc))
        {}

        const char* what() const noexcept override {
            return nested.what();
        }
    };

    using other_result_type = opex::result<gear::TestType, OtherException>;
}

TEST(OrElse, ValidResult)
{
    auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.or_else([](const gear::TestException &exc) {
        return other_result_type::make_exception<OtherException>(exc);
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    ASSERT_NO_THROW(result2.unwrap());
    ASSERT_EQ(result1.unwrap(), result2.unwrap());
}

TEST(OrElse, ValidConstResult)
{
    const auto result1 = my_opex_enabled_function(false);
    const auto result2 = result1.or_else([](const gear::TestException &exc) {
        return other_result_type::make_exception<OtherException>(exc);
    });
    EXPECT_TRUE(result1.is_ok());
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    ASSERT_NO_THROW(result2.unwrap());
    ASSERT_EQ(result1.unwrap(), result2.unwrap());
}

TEST(OrElse, ValidRvalueResult)
{
    auto result1 = my_opex_enabled_function(false);
    EXPECT_TRUE(result1.is_ok());
    ASSERT_NO_THROW(result1.unwrap());
    const auto value = result1.unwrap();

    const auto result2 = std::move(result1).or_else([](gear::TestException &&exc) {
        return other_result_type::make_exception<OtherException>(std::move(exc));
    });
    EXPECT_TRUE(result2.is_ok());
    ASSERT_NO_THROW(result2.unwrap());
    EXPECT_EQ(value, result2.unwrap());
}

TEST(OrElse, InvalidResult)
{
    auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.or_else([](const gear::TestException &exc) {
        return other_result_type::make_exception<OtherException>(exc);
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(result1.what(), result2.what());
}

TEST(OrElse, InvalidConstResult)
{
    const auto result1 = my_opex_enabled_function(true);
    const auto result2 = result1.or_else([](const gear::TestException &exc) {
        return other_result_type::make_exception<OtherException>(exc);
    });
    EXPECT_TRUE(result1.is_err());
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(result1.what(), result2.what());
}

TEST(OrElse, InvalidRvalueResult)
{
    auto result1 = my_opex_enabled_function(true);
    EXPECT_TRUE(result1.is_err());
    const auto what = result1.what();

    const auto result2 = std::move(result1).or_else([](gear::TestException &&exc) {
        return other_result_type::make_exception<OtherException>(std::move(exc));
    });
    EXPECT_TRUE(result2.is_err());
    EXPECT_EQ(what, result2.what());
}
