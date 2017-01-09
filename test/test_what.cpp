#include <gtest/gtest.h>
#include <opex/opex.h>

#include <stdexcept>

TEST(What, NoError) {
    const auto result = opex::call([]() -> int {
       return 0;
    });

    EXPECT_EQ(std::string{}, result.what());
}

TEST(What, StdRuntimeError) {
    const auto message = std::string{"StdRuntimeError"};

    const auto result = opex::call([&message]() -> int {
       throw std::runtime_error{message};
    });

    EXPECT_EQ(message, result.what());
}

TEST(What, StdString) {
    const auto message = std::string{"StdString"};

    const auto result = opex::call<std::string>([&message]() -> int {
       throw message;
    });

    EXPECT_EQ(message, result.what());
}

TEST(What, CharPtr) {
    const auto message = std::string{"CharPtr"};

    const auto result = opex::call<const char*>([&message]() -> int {
       throw message.c_str();
    });

    EXPECT_EQ(message, result.what());
}

TEST(What, Other) {
    const auto result = opex::call<int>([]() -> int {
        throw 0;
    });

    EXPECT_TRUE(result.is_err());
    EXPECT_EQ(std::string{}, result.what());
}
