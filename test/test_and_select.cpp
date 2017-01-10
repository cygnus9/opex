#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

namespace {
    using result_t = opex::result<gear::TestType, gear::TestException>;

    result_t make_exception(const char *msg) {
        return result_t::make_exception<gear::TestException>(msg);
    }
}

TEST(AndSelect, LValue)
{
    const gear::TestType unexpected{};
    result_t lhs{unexpected};

    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, lhs.and_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.and_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.and_select(result_t{gear::TestType{expected}}).unwrap());
}

TEST(AndSelect, CLValue)
{
    const gear::TestType unexpected{};
    const result_t lhs{unexpected};

    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, lhs.and_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.and_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.and_select(result_t{gear::TestType{expected}}).unwrap());
}

TEST(AndSelect, RValue)
{
    const gear::TestType unexpected{};

    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, result_t{unexpected}.and_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, result_t{unexpected}.and_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, result_t{unexpected}.and_select(result_t{gear::TestType{expected}}).unwrap());
}

TEST(AndSelect, LValueErr)
{
    const auto expected = std::string{"expected"};
    result_t lhs = make_exception(expected.c_str());

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, lhs.and_select(rhs_lvalue).what());
    EXPECT_EQ(expected, lhs.and_select(rhs_clvalue).what());
    EXPECT_EQ(expected, lhs.and_select(result_t{gear::TestType{unexpected}}).what());
}

TEST(AndSelect, CLValueErr)
{
    const auto expected = std::string{"expected"};
    const result_t lhs = make_exception(expected.c_str());

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, lhs.and_select(rhs_lvalue).what());
    EXPECT_EQ(expected, lhs.and_select(rhs_clvalue).what());
    EXPECT_EQ(expected, lhs.and_select(result_t{gear::TestType{unexpected}}).what());
}

TEST(AndSelect, RValueErr)
{
    const auto expected = std::string{"expected"};

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, make_exception(expected.c_str()).and_select(rhs_lvalue).what());
    EXPECT_EQ(expected, make_exception(expected.c_str()).and_select(rhs_clvalue).what());
    EXPECT_EQ(expected, make_exception(expected.c_str()).and_select(result_t{gear::TestType{unexpected}}).what());
}
