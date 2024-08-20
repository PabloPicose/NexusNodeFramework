//
// Created by pablo on 5/28/24.
//

#include "TcpSocket.h"
#include "IOContext.h"

#include <arpa/inet.h>

#include "Core/Application.h"

#include <sys/socket.h>
#include <netinet/in.h>


namespace IRC {
    TcpSocket::TcpSocket(Node* parent) :
        IOSocket(parent) {
        //inEvent.connect(boost::bind(&TcpSocket::onInEvent, this));
        outEvent.connect(boost::bind(&TcpSocket::onOutEvent, this));
        rdhupEvent.connect(boost::bind(&TcpSocket::onRdhupEvent, this));
    }

    TcpSocket::~TcpSocket() {
        std::cout << "TcpSocket::~TcpSocket: socket class deleted" << std::endl;
    }

    void TcpSocket::connect(const std::string& host, unsigned short port) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            // TODO check errors
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
            // TODO check errors
            std::cerr << "Invalid address" << std::endl;
            ::close(sockfd);
            return;
        }
        if (!setNonBlocking(sockfd)) {
            // TODO check errors
            std::cerr << "Failed to set non-blocking mode" << std::endl;
            ::close(sockfd);
            return;
        }
        if (::connect(sockfd, (sockaddr*) &addr, sizeof(addr)) == -1) {
            if (errno == EINPROGRESS) {
                // The connection is in progress
                // We need to wait for the socket to be writable
                // We will receive a notification from the IOContext
                // when the socket is writable
                std::cout << "TcpSocket::connect: Connection in progress" << std::endl;
                setFileDescriptor(sockfd);
                modifySocket(sockfd, IOContext::Event::In | IOContext::Event::Out | IOContext::Event::RdHup);
                return;
            } else {
                // TODO check errors
                std::cerr << "Failed to connect" << std::endl;
                ::close(sockfd);
                return;
            }
            return;
        } else {
            // The connection is already established
            std::cout << "TcpSocket::connected: Connected" << std::endl;
            setFileDescriptor(sockfd);
            return;
        }

    }

    unsigned short TcpSocket::getRemotePort() const {
        return 0;
    }

    bool TcpSocket::getIsConnected() const {
        return m_connected;
    }

    void TcpSocket::setConnected(const bool connected) {
        if (connected != m_connected) {
            m_connected = connected;
        }
    }

    bool TcpSocket::setFileDescriptor(const std::optional<int> fd) {
        if (IOSocket::setFileDescriptor(fd)) {
            if (fd.has_value()) {
                if (!modifySocket(fd.value(), IOContext::Event::In | IOContext::Event::RdHup) ) {
                    std::cerr << "TcpSocket::setFileDescriptor: Failed to modify socket" << std::endl;
                    return false;
                }
            }
            return true;
        }
        std::cerr << "TcpSocket::setFileDescriptor: Failed to set file descriptor" << std::endl;
        return false;
    }

    void TcpSocket::close() {
        IOSocket::close();
        setConnected(false);
    }

    void TcpSocket::readData() {
    }

    void TcpSocket::onOutEvent() {
        if (!m_connected) {
            // The connection is established
            std::cout << "TcpSocket::onOutEvent: Connected" << std::endl;
            setConnected(true);
            // unregister the socket from the out event
            modifySocket(getFileDescriptor().value(), IOContext::Event::In | IOContext::Event::RdHup);
        }
    }

    void TcpSocket::onRdhupEvent() {
        // The connection was closed
        std::cout << "TcpSocket::OnRdhupEvent: Connection closed" << std::endl;
        setConnected(false);
        this->close();
    }
    bool TcpSocket::waitForConnected(const int milliseconds) {
        if (!getFileDescriptor().has_value()) {
            std::cerr << "TcpSocket::waitForConnected: TcpSocket without FD" << std::endl;
        }
        if (!getIOContext()) {
            std::cerr << "TcpSocket::waitForConnected: IO Context not created" << std::endl;
        }
        getIOContext()->processFD(getFileDescriptor().value(),
                                  IOContext::Event::Out,
                                  milliseconds);
        return getIsConnected();
    }

} // IRC
