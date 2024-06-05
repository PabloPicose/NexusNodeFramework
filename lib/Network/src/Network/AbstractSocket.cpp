//
// Created by pablo on 5/29/24.
//

#include "AbstractSocket.h"

#include "Network/IOContext.h"
#include <fcntl.h>

IRC::AbstractSocket::~AbstractSocket() {
    this->close();
}

void IRC::AbstractSocket::close() {
    if (m_fd.has_value()) {
        if (auto* ioContext = IOContext::m_instance) {
            ioContext->unregisterSocketInEPoll(m_fd.value());
            // disconnect the signal from the IOContext::eventOccur
        }
        // TODO check the errors
        ::close(m_fd.value());
        m_fd.reset();
        socketClosed(this);
    } else {
        //std::cerr << "AbstractSocket: Attempt to close a socket without file descriptor" << std::endl;
    }
}

std::optional<int> IRC::AbstractSocket::getFileDescriptor() const {
    return m_fd;
}

IRC::IOContext* IRC::AbstractSocket::getIOContext() const {
    return IOContext::m_instance;
}

IRC::AbstractSocket::AbstractSocket(Node* parent) :
    Node(parent) {
    if (!IOContext::m_instance) {
        // Create the IOContext singleton, it will be automatically added as root of the application
        new IOContext();
    }
    if (!IOContext::m_instance) {
        throw std::runtime_error("Failed to create IOContext");
    }
    IOContext::m_instance->eventOccur.
            connect(boost::bind(&AbstractSocket::onEventOcur, this, _1, _2));
}

bool IRC::AbstractSocket::setFileDescriptor(const std::optional<int> fd) {
    m_fd = fd;
    auto* ioContext = IOContext::m_instance;
    if (!ioContext) {
        std::cerr << "AbstractSocket:: IOContext not found" << std::endl;
        return false;
    }
    if (fd.has_value()) {
        return registerSocket(fd.value(), IOContext::Event::In);
    }
    return false;
}

bool IRC::AbstractSocket::setNonBlocking(const int fd) {
    const int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode" << std::endl;
        // TODO check the errors
        return false;
    }
    return true;
}


void IRC::AbstractSocket::update() {
}

bool IRC::AbstractSocket::registerSocket(const int fd, const int events) {
    IOContext* ioContext = IOContext::m_instance;
    if (!ioContext) {
        std::cerr << "IOContext not found" << std::endl;
        return false;
    }
    return ioContext->registerSocketInEPoll(fd, events);
}

bool IRC::AbstractSocket::modifySocket(const int fd, const int events) {
    IOContext* ioContext = IOContext::m_instance;
    if (!ioContext) {
        std::cerr << "IOContext not found" << std::endl;
        return false;
    }
    return ioContext->modifySocketEventsInEPoll(fd, events);
}

bool IRC::AbstractSocket::unregisterSocket(const int fd) {
    IOContext* ioContext = IOContext::m_instance;
    if (!ioContext) {
        std::cerr << "IOContext not found" << std::endl;
        return false;
    }
    return ioContext->unregisterSocketInEPoll(fd);
}

int IRC::AbstractSocket::getFlags() const {
    if (!m_fd.has_value()) {
        std::cerr << "AbstractSocket::getFlags(): File descriptor not set" << std::endl;
        return -1;
    }
    return fcntl(m_fd.value(), F_GETFL, 0);
}

void IRC::AbstractSocket::onEventOcur(const int fd, const int event) {
    if (!m_fd.has_value()) {
        //std::cerr << "AbstractSocket::OnEventOcur(): File descriptor not set" << std::endl;
        return;
    }
    if (fd == m_fd.value()) {
        if (event & IOContext::Event::In) {
            inEvent();
        } else if (event & IOContext::Event::Out) {
            outEvent();
        } else if (event & IOContext::Event::RdHup) {
            rdhupEvent();
        }
    }
}
