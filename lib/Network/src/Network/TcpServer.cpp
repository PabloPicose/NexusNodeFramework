//
// Created by pablo on 5/28/24.
//

#include "TcpServer.h"
#include "Core/Application.h"
#include "Network/TcpSocket.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace IRC {
    TcpServer::TcpServer(Node* parent):
        AbstractSocket(parent) {
        inEvent.connect(boost::bind(&TcpServer::onInEvent, this));
    }

    bool TcpServer::listen(const unsigned short port) {
        if (getFileDescriptor().has_value()) {
            setListening(false);
            this->close();
        }
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            // TODO check errors
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }
        // Under Unix, we want to be able to bind to the port, even if a socket on
        // the same address-port is in TIME_WAIT. Under Windows this is possible
        // anyway -- furthermore, the meaning of reusable on Windows is different:
        // it means that you can use the same address-port for multiple listening
        // sockets.
        // Don't abort though if we can't set that option. For example the socks
        // engine doesn't support that option, but that shouldn't prevent us from
        // trying to bind/listen.
        int optval = 1;
        if (setsockopt(server_fd,
                       SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) < 0) {
            std::cerr << "Failed to REUSE ADDR option on the server socket" << std::endl;
        }
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(server_fd, (sockaddr*) &addr, sizeof(addr)) == -1) {
            // TODO check errors
            ::close(server_fd);
            return false;
        }
        if (::listen(server_fd, m_maxClients) == -1) {
            // TODO check errors
            std::cerr << "Failed to listen on socket" << std::endl;
            ::close(server_fd);
            return false;
        }
        if (!setNonBlocking(server_fd)) {
            // TODO check errors
            std::cerr << "Failed to set non-blocking mode" << std::endl;
            ::close(server_fd);
            return false;
        }

        setFileDescriptor(server_fd);
        m_port = port;
        setListening(true);

        return true;
    }

    TcpServer::~TcpServer() {
    }

    void TcpServer::close() {
        AbstractSocket::close();
        m_port.reset();
        setListening(false);
    }

    int TcpServer::getMaxClients() const {
        return m_maxClients;
    }

    void TcpServer::setMaxClients(const int maxClients) {
        m_maxClients = maxClients;
    }

    TcpSocket* TcpServer::nextPendingConnection() {
        if (!m_isListening) {
            return nullptr;
        }
        sockaddr_in client_addr = {};
        socklen_t client_addr_len = sizeof(client_addr);
        const int client_fd = accept(getFileDescriptor().value(),
                                     reinterpret_cast<sockaddr*>(&client_addr),
                                     &client_addr_len);
        if (client_fd == -1) {
            // TODO check errors
            //std::cerr << "Failed to accept new connection" << std::endl;
            return nullptr;
        }
        if (!setNonBlocking(client_fd)) {
            // TODO check errors
            std::cerr << "TcpServer::NextPendingConnection: Failed to set non-blocking mode" << std::endl;
            ::close(client_fd);
            return nullptr;
        }
        std::cout << "TcpServer::nextPendingConnection: New connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        TcpSocket* socket = new TcpSocket(this);
        if (!socket->setFileDescriptor(client_fd)) {
            delete socket;
            return nullptr;
        }
        socket->setConnected(true);
        return socket;
    }

    std::optional<unsigned short> TcpServer::getPort() const {
        return m_port;
    }

    bool TcpServer::isListening() const {
        return m_isListening;
    }


    void TcpServer::setListening(const bool listening) {
        if (listening != m_isListening) {
            m_isListening = listening;
        }
    }

    void TcpServer::onInEvent() {
        newConnection();
    }

} // IRC
