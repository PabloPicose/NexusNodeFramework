# tests with GTests

find_package(GTest REQUIRED)
enable_testing()

add_executable(core_tests
        core_test.cpp
)

target_link_libraries(core_tests
        IRCCore
        GTest::GTest
        GTest::Main
)

add_test(core_tests core_tests)
