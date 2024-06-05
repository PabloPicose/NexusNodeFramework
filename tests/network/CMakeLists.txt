# tests with GTests

find_package(GTest REQUIRED)
enable_testing(true)

add_executable(network_tests
        network_test.cpp
)

target_link_libraries(network_tests
        IRCNetwork
        GTest::GTest
        GTest::Main
)

add_test(network_tests network_tests )