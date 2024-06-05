//
// Created by pablo on 5/29/24.
//
#include <iostream>
#include <cstring> // for memset and strlen
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // for inet_pton
#include <unistd.h> // for close()
#include <array>

const char* server_ip = "127.0.0.1";
const int server_port = 8080;

int main(int argc, char** argv) {
    // Create a socket, and connect it to localhost:8080
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    // connect to server
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);
    if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(sockfd);
        return 1;
    }
    std::cout << "Connected to server" << std::endl;

    const char* message = "Hello, World! How are you? I expect this message to be echoed back to me.";
    std::cout << "Message size: " << strlen(message) << std::endl;
    if (send(sockfd, message, strlen(message), 0) == -1) {
        std::cerr << "Failed to send message" << std::endl;
        close(sockfd);
        return 1;
    } else {
        std::cout << "Message sent" << std::endl;
    }

    while (true) {
        // read the message from the server
        std::array<char, 1024> buffer;
        std::memset(buffer.data(), 0, buffer.size());
        const int bytes_read = read(sockfd, buffer.data(), buffer.size());
        if (bytes_read == -1) {
            std::cerr << "Failed to read data" << std::endl;
            break;
        } else if (bytes_read == 0) {
            std::cerr << "Connection closed by server" << std::endl;
            break;
        } else {
            std::cout << "Received: " << buffer.data() << std::endl;
        }
        // sleep for 5 seconds
        sleep(5);
    }


    return 0;
}