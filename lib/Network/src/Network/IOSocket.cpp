//
// Created by pablo on 5/30/24.
//

#include "IOSocket.h"

#include <sys/socket.h>

namespace IRC {
    IOSocket::~IOSocket() {}

    int64_t IOSocket::write(const char* data, const std::size_t max_size) {
        const auto optfd = getFileDescriptor();
        if (optfd.has_value()) {
            const int fd = optfd.value();
            const int bytes_written = send(fd, data, max_size, 0);
            if (bytes_written == -1) {
                std::cerr << "Failed to write data" << std::endl;
            }
            return bytes_written;
        }
        return -1;
    }

    int64_t IOSocket::write(const std::vector<char>& data) { return write(data.data(), data.size()); }

    int64_t IOSocket::write(const char* data) { return write(data, std::strlen(data)); }

    std::vector<char> IOSocket::readAll() {
        std::vector<char> buffer;
        auto fd_opt = getFileDescriptor();
        if (fd_opt.has_value()) {
            const int fd = fd_opt.value();
            while (true) {
                std::array<char, 1024> in_buff;
                std::memset(in_buff.data(), 0, in_buff.size());
                const int bytes_read = read(fd, in_buff.data(), in_buff.size());
                if (bytes_read == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::cout << "No more data to read" << std::endl;
                        break;
                    } else {
                        std::cerr << "Failed to read data" << std::endl;
                        break;
                    }
                }
                if (bytes_read == 0) {
                    // EOF means client disconnected
                    std::cout << "IOSocket::readAll(): Client disconnected" << std::endl;
                    close();
                    break;
                }
                // data received
                std::cout << "Received " << bytes_read << " bytes: " << in_buff.data() << std::endl;
                // copy at the end of the buffer
                buffer.insert(buffer.end(), in_buff.begin(), in_buff.begin() + bytes_read);
            }
        }
        return buffer;
    }

    IOSocket::IOSocket(Node* parent) : AbstractSocket(parent) {
        inEvent.connect(boost::bind(&IOSocket::onInEvent, this));
    }

    void IOSocket::onInEvent() {
        std::cout << "IOSocket::onInEvent()" << std::endl;
        const auto data = readAll();
        if (!data.empty()) {
            readyRead(this, data);
        }
    }

} // namespace IRC
