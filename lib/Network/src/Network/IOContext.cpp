//
// Created by pablo on 5/29/24.
//

#include "IOContext.h"

#include <fcntl.h>


namespace IRC {

    IOContext* IOContext::m_instance = nullptr;

    IOContext::~IOContext() {
        m_instance = nullptr;
        if (m_fd.has_value()) {
            close(m_fd.value());
        }
        std::cout << "IOContext destroyed" << std::endl;
    }

    std::optional<int> IOContext::getFD() const {
        return m_fd;
    }

    void IOContext::update() {
        if (m_fd_count > 0) {
            const int n = epoll_wait(m_fd.value(), m_events.data(), m_events.size(), 0);
            for (int i = 0; i < n; i++) {
                const auto& ev = m_events[i];
                if (ev.events & EPOLLIN) {
                    eventOccur(ev.data.fd, In);
                }
                if (ev.events & EPOLLOUT) {
                    eventOccur(ev.data.fd, Out);
                }
                if (ev.events & EPOLLRDHUP) {
                    eventOccur(ev.data.fd, RdHup);
                }
            }
        }
    }

    IOContext::IOContext(Node* parent) :
        Node(parent) {
        if (!m_instance) {
            m_instance = this;
        } else {
            throw std::runtime_error("IOContext already exists");
        }
        // Create the epoll_fd
        const int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            throw std::runtime_error("Failed to create epoll_fd");
        }
        m_fd = epoll_fd;
        std::cout << "IOContext created" << std::endl;
    }

    bool IOContext::registerSocketInEPoll(const int fd, const EventFlags events) {
        if (!m_fd.has_value()) {
            std::cerr << "IOContext not initialized" << std::endl;
            return false;
        }
        int epoll_flags = 0;
        if (events & In) {
            epoll_flags |= EPOLLIN;
        }
        if (events & Out) {
            epoll_flags |= EPOLLOUT;
        }
        struct epoll_event ev;
        ev.events = epoll_flags;
        ev.data.fd = fd;
        if (epoll_ctl(m_fd.value(), EPOLL_CTL_ADD, fd, &ev) == -1) {
            // TODO log error
            std::cerr << "Failed to register socket" << std::endl;
            return false;
        }
        m_fd_count++;
        return true;
    }

    bool IOContext::modifySocketEventsInEPoll(const int fd, const EventFlags events) {
        int epoll_flags = 0;
        if (events & In) {
            epoll_flags |= EPOLLIN;
        }
        if (events & Out) {
            epoll_flags |= EPOLLOUT;
        }
        if (events & RdHup) {
            epoll_flags |= EPOLLRDHUP | EPOLLHUP;
        }
        struct epoll_event ev;
        ev.events = epoll_flags;
        ev.data.fd = fd;
        if (epoll_ctl(m_fd.value(), EPOLL_CTL_MOD, fd, &ev) == -1) {
            // TODO log error
            std::cerr << "Failed to modify socket" << std::endl;
            return false;
        }
        return true;
    }

    bool IOContext::unregisterSocketInEPoll(const int fd) {
        if (epoll_ctl(m_fd.value(), EPOLL_CTL_DEL, fd, nullptr) == -1) {
            // TODO log error
            return false;
        }
        m_fd_count--;
        return true;
    }
} // IRC
