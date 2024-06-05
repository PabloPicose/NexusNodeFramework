cmake_minimum_required(VERSION 3.28)
project(POACS)

enable_testing(true)

#find_package(Crow CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(POACS main.cpp)

#target_link_libraries(POACS PRIVATE Boost::system Boost::thread Boost::asio)

add_subdirectory(lib/Core)
add_subdirectory(lib/Network)

target_link_libraries(POACS PRIVATE IRCCore IRCNetwork)
#target_link_libraries(POACS PRIVATE Crow::Crow)

add_subdirectory(tests)

## TEST CLIENT
add_executable(POACS_TEST_CLIENT test_client.cpp)

