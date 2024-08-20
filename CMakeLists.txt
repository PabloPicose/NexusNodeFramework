cmake_minimum_required(VERSION 3.28)
project(NexusNodeFramework)

enable_testing(true)

set(CMAKE_CXX_STANDARD 20)

add_executable(POACS main.cpp)


add_subdirectory(lib/Core)
add_subdirectory(lib/Network)
add_subdirectory(lib/Graphics)

target_link_libraries(POACS PRIVATE IRCCore IRCNetwork)

add_subdirectory(tests)

add_executable(graphics_test test_graphics.cpp)
target_link_libraries(graphics_test IRCGraphics)
