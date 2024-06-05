cmake_minimum_required(VERSION 3.28)
project(IRCNetwork)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS signals2)
find_package(SFML COMPONENTS network REQUIRED)

add_library(IRCNetwork STATIC
        src/Network/TcpServer.h
        src/Network/TcpServer.cpp
        src/Network/TcpSocket.cpp
        src/Network/TcpSocket.h
        src/Network/IOContext.cpp
        src/Network/IOContext.h
        src/Network/AbstractSocket.cpp
        src/Network/AbstractSocket.h
        src/Network/IOSocket.cpp
        src/Network/IOSocket.h
)


target_include_directories(IRCNetwork PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#target_link_libraries(IRCNetwork PUBLIC Boost::system Boost::thread Boost::asio Boost::signals2)
target_link_libraries(IRCNetwork PUBLIC IRCCore)
target_link_libraries(IRCNetwork PUBLIC sfml-network)