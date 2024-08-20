cmake_minimum_required(VERSION 3.28)
project(IRCGraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find glfw3 package
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)

add_library(IRCGraphics STATIC
        src/Graphics/MainWindow.cpp
        src/Graphics/MainWindow.h
)


target_include_directories(IRCGraphics PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(IRCGraphics PUBLIC IRCCore)
target_link_libraries(IRCGraphics PUBLIC glfw)
target_link_libraries(IRCGraphics PUBLIC glad::glad)
target_link_libraries(IRCGraphics PUBLIC  OpenGL::GL)
