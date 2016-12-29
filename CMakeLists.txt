cmake_minimum_required(VERSION 2.9)
project(opex LANGUAGES CXX)


add_library(opex INTERFACE)
target_include_directories(opex INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS opex EXPORT opexConfig)
install(DIRECTORY include DESTINATION include)
install(EXPORT opexConfig DESTINATION share/opex/cmake)
export(TARGETS opex FILE opexConfig.cmake)


find_package(GTest)

if(${GTEST_FOUND})
    add_executable(test_opex
        test/gear.cpp
        test/test_accessors.cpp
        test/test_call.cpp
        test/test_construct.cpp
        test/test_map.cpp
    )
    set_target_properties(test_opex PROPERTIES
        CXX_STANDARD 11
    )
    target_include_directories(test_opex PRIVATE
        ${GTEST_INCLUDE_DIRS}
    )
    target_link_libraries(test_opex
        opex
        GTest::GTest
        GTest::Main
    )

    enable_testing()
    add_test(test_opex test_opex)
endif()
