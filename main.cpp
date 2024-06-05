#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "Core/Application.h"
#include "Network/TcpServer.h"
#include "Network/TcpSocket.h"
#include "Core/NodePtr.h"

using namespace IRC;

class MyServer : public TcpServer {
public:
    MyServer(Node* parent = nullptr) :
        TcpServer(parent) {
        newConnection.connect(boost::bind(&MyServer::onNewConnection, this));
    }

    void onNewConnection() {
        TcpSocket* socket = nextPendingConnection();
        if (!socket) {
            std::cerr << "Failed to get new connection" << std::endl;
            return;
        }
        socket->readyRead.connect(boost::bind(&MyServer::onReadyRead, this, _1, _2));
        socket->socketClosed.connect(boost::bind(&MyServer::onSocketClosed, this, _1));

        sock = socket;
    }

    void onReadyRead(IOSocket* emitter, const std::vector<char>& data) {
        std::cout << "pepe" << std::endl;
        std::cout << "MySocket::onReadyRead(): " << data.data() << std::endl;
        sock->write("hola");
    }

    void onSocketClosed(AbstractSocket* socket) {
        std::cout << "MySocket: Socket closed" << std::endl;
        if (sock) {
            sock->deleteLater();
            sock = nullptr;
            std::cout << "MySocket: Socket delete later" << std::endl;
        }
    }

    TcpSocket* sock = nullptr;
};


// TEST SCOKET
void set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags" << std::endl;
        return;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set socket flags" << std::endl;
    }
}

int create_server_socket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*) &addr, sizeof(addr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return -1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -1;
    }

    set_non_blocking(server_fd);
    return server_fd;
}

void handle_client_event_read(const int fd_client) {
    std::vector<char> buffer;

    constexpr int max_read = 76;

    // read until all the data is read
    while (true) {
        std::array<char, max_read> in_buff;
        std::memset(in_buff.data(), 0, in_buff.size());
        const int bytes_read = read(fd_client, in_buff.data(), in_buff.size());
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "No more data to read" << std::endl;
                break;
            } else {
                std::cerr << "Failed to read data" << std::endl;
                break;
            }
        } else if (bytes_read == 0) {
            // EOF means client disconnected
            std::cout << "Client disconnected" << std::endl;
            close(fd_client);
            break;
        } else {
            // data received
            std::cout << "Received " << bytes_read << " bytes: " << in_buff.data() << std::endl;
            // copy at the end of the buffer
            buffer.insert(buffer.end(), in_buff.begin(), in_buff.begin() + bytes_read);
        }
    }
    std::cout << "Full message: " << buffer.data() << "\n";
    return;
}

void handle_client_event_write(int fd_client, std::vector<char>& data, size_t& data_offset) {
    while (data_offset < data.size()) {
        ssize_t bytes_sent = send(fd_client, data.data() + data_offset, data.size() - data_offset, MSG_DONTWAIT);
        if (bytes_sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Output buffer is full, exit the function and wait for the next EPOLLOUT event
                std::cerr << "Output buffer is full, try again later" << std::endl;
                break;
            } else {
                // Handle other errors
                std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
                close(fd_client);
                return;
            }
        }
        data_offset += bytes_sent;
    }

    // If all data has been sent, reset data_offset or handle as needed
    if (data_offset == data.size()) {
        data_offset = 0; // Reset offset for new data
        data.clear(); // Optionally clear the data if all has been sent
    }
}

#include <csignal>

void signal_handler(int signal) {
    std::cout << "Signal received: " << signal << std::endl;
    if (signal == SIGINT) {
        std::cout << "Exiting..." << std::endl;
        Application::instance()->quit();
        delete Application::instance();
    }
    exit(0);
}


void test() {
    MyServer* test = new MyServer();
    NodePtr<MyServer> test_ptr(test);
    // check if the pointer is valid
    if (test_ptr) {
        std::cout << "Pointer is valid" << std::endl;
    } else {
        std::cerr << "Pointer is invalid" << std::endl;
    }
    // check if the pointer is null
    if (test_ptr) {
        std::cout << "Pointer is not null" << std::endl;
    } else {
        std::cerr << "Pointer is null" << std::endl;
    }
    // check if the pointer is valid
    if (test_ptr.isAlive()) {
        std::cout << "Pointer is alive" << std::endl;
    } else {
        std::cerr << "Pointer is not alive" << std::endl;
    }
    // check if the pointer is marked to delete (should be false)
    if (test_ptr.isMarkedToDelete()) {
        std::cerr << "Pointer is marked to delete" << std::endl;
    } else {
        std::cout << "Pointer is not marked to delete" << std::endl;
    }
    // mark the pointer to delete
    test_ptr->deleteLater();
    // check if the pointer is marked to delete (should be true)
    if (test_ptr.isMarkedToDelete()) {
        std::cout << "Pointer is marked to delete" << std::endl;
    } else {
        std::cerr << "Pointer is not marked to delete" << std::endl;
    }
    // check if the pointer is valid (should be true)
    if (test_ptr) {
        std::cout << "Pointer is valid" << std::endl;
    } else {
        std::cerr << "Pointer is invalid" << std::endl;
    }
    // delete the pointer
    delete test;
    // check if the pointer is valid (should be false)
    if (test_ptr) {
        std::cerr << "Pointer is valid" << std::endl;
    } else {
        std::cout << "Pointer is invalid" << std::endl;
    }
    // check if the pointer is marked to delete (should be true)
    if (test_ptr.isMarkedToDelete()) {
        std::cout << "Pointer is marked to delete" << std::endl;
    } else {
        std::cerr << "Pointer is not marked to delete" << std::endl;
    }
    // check if the pointer is alive (should be false)
    if (test_ptr.isAlive()) {
        std::cerr << "Pointer is alive" << std::endl;
    } else {
        std::cout << "Pointer is not alive" << std::endl;
    }
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    return 0;

    IRC::Application* app = new IRC::Application();

    test();

    MyServer server;
    server.setMaxClients(1);
    bool ok = server.listen(8080);
    if (!ok) {
        std::cerr << "Failed to listen on port 8080" << std::endl;
        return 1;
    }

    app->run();

    delete app;
    return 0;
    // TEST SOCKET
    int server_fd = create_server_socket(8080);
    if (server_fd == -1) {
        return 1;
    }
    // epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Failed to create epoll" << std::endl;
        close(server_fd);
        return 1;
    }
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        std::cerr << "Failed to add server socket to epoll" << std::endl;
        close(server_fd);
        close(epoll_fd);
        return 1;
    }
    constexpr int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];

    // the epoll should not block the main thread
    while (true) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, 0);
        if (n == -1) {
            std::cerr << "Failed to wait for events" << std::endl;
            close(server_fd);
            close(epoll_fd);
            return 1;
        }
        if (n != 0) {
            std::cout << "Event received count: " << n << std::endl;
            for (int i = 0; i < n; ++i) {
                if (events[i].data.fd == server_fd) {
                    // accept new client
                    sockaddr_in client_addr = {};
                    socklen_t client_addr_len = sizeof(client_addr);
                    const int client_fd = accept(server_fd, (sockaddr*) &client_addr, &client_addr_len);
                    if (client_fd < 0) {
                        std::cerr << "Failed to accept new connection" << std::endl;
                        continue;
                    }
                    set_non_blocking(client_fd);
                    // add client to epoll
                    epoll_event client_event = {};
                    client_event.events = EPOLLIN | EPOLLET | EPOLLOUT;
                    client_event.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
                        std::cerr << "Failed to add client to epoll" << std::endl;
                        close(client_fd);
                    }
                    // Send welcome message
                    const char* welcome_message = "Welcome to the server!";
                    if (send(client_fd, welcome_message, strlen(welcome_message), 0) == -1) {
                        std::cerr << "Failed to send welcome message" << std::endl;
                        // close(client_fd);
                    } else {
                        std::cout << "Welcome message sent" << std::endl;
                    }
                } else {
                    // Check if it is a read event
                    if (events[i].events & EPOLLIN) {
                        handle_client_event_read(events[i].data.fd);
                    } else if (events[i].events & EPOLLOUT) {
                        // Handle write event
                        std::cout << "Write event received" << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}
