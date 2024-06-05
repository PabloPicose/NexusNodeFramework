//
// Created by pablo on 5/28/24.
//

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Network/AbstractSocket.h"

namespace sf {
    class TcpListener;
}

namespace IRC {
    class TcpSocket;

    class TcpServer : public AbstractSocket {
    public:
        explicit TcpServer(Node* parent = nullptr);

        bool listen(unsigned short port);

        ~TcpServer() override;

        void close() override;

        int getMaxClients() const;

        void setMaxClients(int maxClients);

        /**
         * Gets the next client connection already accepted. This class by default takes the ownership of the
         * TcpSocket.
         * @return A valid pointer to a TcpSocket if there is a new connection available, otherwise nullptr.
         */
        TcpSocket* nextPendingConnection();

        std::optional<unsigned short> getPort() const;

        bool isListening() const;

        /**
         * @brief Signal emitted when a new connection is available.
         * @details This signal is emitted when a new connection is available.
         * When the signal is called, the nextPendingConnection() function can be called to get the new connection.
         */
        boost::signals2::signal<void()> newConnection;

    private:
        void setListening(bool listening);

        void onInEvent();

    public:

    private:
        std::vector<TcpSocket*> m_clients;
        bool m_isListening = false;
        int m_maxClients = 30;
        std::optional<unsigned short> m_port;
    };
} // IRC

#endif //TCPSERVER_H
