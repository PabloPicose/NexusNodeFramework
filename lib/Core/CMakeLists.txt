cmake_minimum_required(VERSION 3.28)
project(IRCCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS signals2 asio)

add_library(IRCCore STATIC
        src/Core/Application.cpp
        src/Core/Application.h
        src/Core/pch.h
        src/Core/Node.cpp
        src/Core/Node.h
        src/Core/NodePtr.cpp
        src/Core/NodePtr.h
        src/Core/Timer.h
        src/Core/Timer.cpp
)

target_precompile_headers(IRCCore PUBLIC
        src/Core/pch.h
)

target_include_directories(IRCCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(IRCCore PUBLIC Boost::signals2 Boost::asio)
