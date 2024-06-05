//
// Created by pablo on 6/3/24.
//
#include <chrono>
#include <gtest/gtest.h>
#include "Core/Application.h"
#include "Network/TcpServer.h"
#include "Network/TcpSocket.h"
#include "Network/IOContext.h"

using namespace IRC;

class TestTcpServer final : public TcpServer {
public:
    explicit TestTcpServer(Node* parent = nullptr) :
        TcpServer(parent) {
        newConnection.connect([this]() {
            signalNewConnectionCount++;
        });
    }


    int signalNewConnectionCount = 0;
    int signalSocketClosedCount = 0;

};

class TestTcpSocket final : public TcpSocket {
public:
    explicit TestTcpSocket(Node* parent = nullptr) :
        TcpSocket(parent) {
        socketClosed.connect([this](AbstractSocket*) {
            signalSocketClosedCount++;
        });
        // Connect the readyRead signal to the onReadyRead slot
        readyRead.connect(boost::bind(&TestTcpSocket::onReadyRead, this, _1, _2));
    }

    void onReadyRead(IOSocket*, const std::vector<char>& dataIn) {
        signalReadyReadCount++;
        data = dataIn;
    }

    int signalSocketClosedCount = 0;
    int signalReadyReadCount = 0;

    std::vector<char> data;

};

class NetworkFixture : public ::testing::Test {
protected:
    void SetUp() override {
        app = new Application();
    }

    void TearDown() override {
        if (app) {
            delete app;
            app = nullptr;
        }
    }

    Application* app = nullptr;
};

TEST_F(NetworkFixture, AppInitializer) {
    EXPECT_EQ(Application::instance(), app);
}

TEST_F(NetworkFixture, TcpServerEmptyInitializer) {
    TcpServer server;
    EXPECT_FALSE(server.isListening());
    EXPECT_FALSE(server.getPort().has_value());
    EXPECT_FALSE(server.nextPendingConnection());
    EXPECT_FALSE(server.parent());
    EXPECT_TRUE(server.isRoot());
    EXPECT_FALSE(server.getFileDescriptor().has_value());
}

TEST_F(NetworkFixture, TcpServerContextCreated) {
    TcpServer server;
    EXPECT_TRUE(server.getIOContext());
}

TEST_F(NetworkFixture, TcpServerListen) {
    TcpServer server;
    EXPECT_TRUE(server.listen(1234));
    EXPECT_TRUE(server.getFileDescriptor().has_value());
    EXPECT_TRUE(server.isListening());
    EXPECT_EQ(server.getPort().value(), 1234);
    server.close();
    EXPECT_FALSE(server.isListening());
    EXPECT_FALSE(server.getPort().has_value());
}

TEST_F(NetworkFixture, TcpServerListenTwice) {
    TcpServer server;
    EXPECT_TRUE(server.listen(1234));
    EXPECT_TRUE(server.isListening());
    EXPECT_EQ(server.getPort().value(), 1234);

    // Call other listen should close the previous one
    EXPECT_TRUE(server.listen(1235));
    EXPECT_TRUE(server.isListening());
    EXPECT_EQ(server.getPort().value(), 1235);
    server.close();
}

TEST_F(NetworkFixture, TwoTcpServerListenSamePort) {
    TcpServer server;
    EXPECT_TRUE(server.listen(1234));
    EXPECT_TRUE(server.isListening());
    TcpServer server2;
    EXPECT_FALSE(server2.listen(1234));
    EXPECT_FALSE(server2.isListening());
    EXPECT_FALSE(server2.getPort().has_value());
}

TEST_F(NetworkFixture, TcpServerMaxClients) {
    TcpServer server;
    EXPECT_TRUE(server.listen(1234));
    EXPECT_EQ(server.getMaxClients(), 30);
    server.setMaxClients(10);
    EXPECT_EQ(server.getMaxClients(), 10);
    server.close();
}

TEST_F(NetworkFixture, TcpServerNextPendingConnection) {
    TestTcpServer server;
    EXPECT_TRUE(server.listen(1234));
    EXPECT_EQ(server.signalNewConnectionCount, 0);
    auto ioContext = server.getIOContext();
    EXPECT_TRUE(ioContext);

    TestTcpSocket socketClient;
    socketClient.connect("127.0.0.1", 1234);
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    auto end_time = start + seconds(1);
    // in two seconds, calling in the event loop the server.run() and socket.run() and IOContext::run()
    // may be enough to get the new connection
    TcpSocket* socketServer = nullptr;
    while (high_resolution_clock::now() < end_time) {
        // The order to run is important, first the IOContext, then the server and finally the socket
        //ioContext->run();
        //server.run();
        //socketClient.run();
        app->loopOnce();
        if (!socketServer) {
            socketServer = server.nextPendingConnection();
        } else {
            EXPECT_FALSE(server.nextPendingConnection());
        }
    }
    EXPECT_EQ(server.signalNewConnectionCount, 1);
    EXPECT_TRUE(socketClient.getIsConnected());
    EXPECT_TRUE(socketServer->getIsConnected());
    EXPECT_EQ(socketServer->parent(), &server);
    EXPECT_EQ(server.childrenCount(), 1);
    EXPECT_TRUE(socketServer);
    EXPECT_TRUE(socketServer->getFileDescriptor().has_value());
    EXPECT_TRUE(socketClient.getFileDescriptor().has_value());
    socketServer->setName("SocketServer");
    socketClient.setName("SocketClient");

    // Check what happends when the server sends a message to the client
    const char* message = "Hello";
    EXPECT_EQ(socketServer->write(message, 5), 5);
    // Start a new loop to check the message
    start = high_resolution_clock::now();
    end_time = start + seconds(1);
    while (high_resolution_clock::now() < end_time) {
        app->loopOnce();
    }
    // The signal readyRead should be emitted once and the data should be the same as the message
    EXPECT_EQ(socketClient.signalReadyReadCount, 1);
    EXPECT_EQ(socketClient.data.size(), 5);

    // Now disconnect the socketServer->close() and check if the signal socketClosed is emitted
    socketServer->close();
    start = high_resolution_clock::now();
    end_time = start + seconds(1);
    while (high_resolution_clock::now() < end_time) {
        app->loopOnce();
    }
    EXPECT_EQ(socketClient.signalSocketClosedCount, 1);
    EXPECT_FALSE(socketClient.getIsConnected());
    EXPECT_FALSE(socketServer->getFileDescriptor().has_value());
    EXPECT_FALSE(socketClient.getFileDescriptor().has_value());
}
