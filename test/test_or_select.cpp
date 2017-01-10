#include <gtest/gtest.h>
#include <opex/opex.h>

#include "gear.h"

namespace {
    using result_t = opex::result<gear::TestType, gear::TestException>;

    result_t make_exception() {
        return result_t::make_exception<gear::TestException>("whoops");
    }
}

TEST(OrSelect, LValue)
{
    const gear::TestType expected{};
    result_t lhs{expected};

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, lhs.or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(result_t{gear::TestType{unexpected}}).unwrap());
}

TEST(OrSelect, CLValue)
{
    const gear::TestType expected{};
    const result_t lhs{expected};

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, lhs.or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(result_t{gear::TestType{unexpected}}).unwrap());
}

TEST(OrSelect, RValue)
{
    const gear::TestType expected{};

    const gear::TestType unexpected{};
    result_t rhs_lvalue{unexpected};
    const result_t rhs_clvalue{unexpected};

    EXPECT_EQ(expected, result_t{expected}.or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, result_t{expected}.or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, result_t{expected}.or_select(result_t{gear::TestType{unexpected}}).unwrap());
}

TEST(OrSelect, LValueErr)
{
    result_t lhs = make_exception();

    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, lhs.or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(result_t{gear::TestType{expected}}).unwrap());
}

TEST(OrSelect, CLValueErr)
{
    const result_t lhs = make_exception();

    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, lhs.or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, lhs.or_select(result_t{gear::TestType{expected}}).unwrap());
}

TEST(OrSelect, RValueErr)
{
    const gear::TestType expected{};
    result_t rhs_lvalue{expected};
    const result_t rhs_clvalue{expected};

    EXPECT_EQ(expected, make_exception().or_select(rhs_lvalue).unwrap());
    EXPECT_EQ(expected, make_exception().or_select(rhs_clvalue).unwrap());
    EXPECT_EQ(expected, make_exception().or_select(result_t{gear::TestType{expected}}).unwrap());
}
