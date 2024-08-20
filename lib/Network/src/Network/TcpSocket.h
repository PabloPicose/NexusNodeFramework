//
// Created by pablo on 5/28/24.
//

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Network/IOSocket.h"

namespace IRC {
    class TcpSocket : public IOSocket {
    public:
        explicit TcpSocket(Node* parent = nullptr);

        ~TcpSocket() override;

        void connect(const std::string& host, unsigned short port) override;

        bool waitForConnected(int milliseconds = -1);

        unsigned short getRemotePort() const;

        using signal_type = boost::signals2::signal<void()>;

        bool getIsConnected() const;

        /**
         * @brief Close the connection with the peer. This function is called by the destructor too.
         * After closing the connection, this socket will not be longer connected.
         */
        void close() override;

    protected:
        void setConnected(bool connected);

        bool setFileDescriptor(std::optional<int> fd) override;

    private:
        void readData();

        void onOutEvent();

        void onRdhupEvent();

    private:
        friend class TcpServer;
        std::vector<char> m_readBuffer;
        bool m_connected = false;
    };
} // namespace IRC

#endif // TCPSOCKET_H
