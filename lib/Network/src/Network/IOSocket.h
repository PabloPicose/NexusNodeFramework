//
// Created by pablo on 5/30/24.
//

#ifndef IOSOCKET_H
#define IOSOCKET_H

#include "Network/AbstractSocket.h"

namespace IRC {

    class IOSocket : public AbstractSocket {
    public:
        ~IOSocket() override;

        /**
         * Writes at most maxSize bytes of data from data to the device.
         * @param data the data to write.
         * @param max_size the maximum number of bytes to write.
         * @return the number of bytes that were actually written, or -1 if an error occurred.
         */
        int64_t write(const char* data, std::size_t max_size);

        /**
         * Writes the content of data to the device.
         * @param data the data to write.
         * @return the number of bytes that were actually written, or -1 if an error occurred.
         */
        int64_t write(const std::vector<char>& data);

        /**
         * Writes data from a zero-terminated string of 8-bit characters to the device.
         * @param data the data to write.
         * @return the number of bytes that were actually written, or -1 if an error occurred.
         */
        int64_t write(const char* data);

        virtual void connect(const std::string& host, unsigned short port) = 0;

        //! Signal emitted when data is available to read. The data is passed
        //! through the signal and not stored.
        boost::signals2::signal<void(IOSocket* emitter, const std::vector<char>& dataIn)> readyRead;

    protected:
        explicit IOSocket(Node* parent = nullptr);

    private:
        void onInEvent();

        std::vector<char> readAll();

    private:
        std::vector<char> m_readBuffer;
    };

} // IRC

#endif //IOSOCKET_H
