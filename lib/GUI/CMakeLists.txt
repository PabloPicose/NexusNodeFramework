cmake_minimum_required(VERSION 3.28)
project(IRCGui)

# find glfw3 package
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)

find_package(imgui CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(IRCGui STATIC
    src/GUI/GUIWindow.cpp 
    src/GUI/GUIWindow.h 
)


target_include_directories(IRCGui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(IRCGui PUBLIC IRCCore)
target_link_libraries(IRCGui PUBLIC imgui::imgui)
target_link_libraries(IRCGui PRIVATE OpenGL::GL glad::glad glfw)
