#pragma once

#include <exception>
#include <memory>

#include <opex/opex.h>


namespace gear {
    class TestType {
        static unsigned s_instanceid;
        unsigned m_instanceid;

        static constexpr unsigned UNSET = unsigned(-1);

    public:
        TestType(): m_instanceid(s_instanceid++)
        {}

        TestType(const TestType &other): m_instanceid(other.m_instanceid)
        {}

        TestType(TestType &&other): m_instanceid(other.m_instanceid)
        {
            other.m_instanceid = UNSET;
        }

        TestType& operator=(const TestType &other) {
            if (std::addressof(other) != this)
                m_instanceid = other.m_instanceid;
            return *this;
        }

        TestType& operator=(TestType &&other) {
            if (std::addressof(other) != this) {
                m_instanceid = other.m_instanceid;
                other.m_instanceid = UNSET;
            }
            return *this;
        }

        ~TestType() = default;

        bool operator==(const TestType &other) const noexcept {
            return other.m_instanceid == m_instanceid;
        }

        bool operator!=(const TestType &other) const noexcept {
            return !(*this == other);
        }

        bool valid() const noexcept {
            return m_instanceid != UNSET;
        }

        unsigned id() const noexcept {
            return m_instanceid;
        }
    };

    class TestException : public std::runtime_error {
    public:
        explicit TestException(const char *message):
                std::runtime_error(message)
        {}
    };

    using TestResult = opex::result<gear::TestType, gear::TestException>;

    gear::TestType throw_if_true(bool b);
}
