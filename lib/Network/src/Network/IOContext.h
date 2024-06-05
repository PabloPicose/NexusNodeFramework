//
// Created by pablo on 5/29/24.
//

#ifndef IOCONTEXT_H
#define IOCONTEXT_H

#include "Core/Node.h"
#include <sys/epoll.h>

struct epoll_event;

namespace IRC {
    class IOContext final : public Node {
    public:
        enum Event {
            //! The associated file is available for read(2) operations.
            In = (1 << 0),
            //! The associated file is available for write(2) operations.
            //! The socket is ready for writing.
            Out = (1 << 1),
            /**
             * This flag is specifically useful to detect when a peer has
             * closed the connection or shut down the writing half of
             * the connection. It's an essential flag for accurately
             * detecting closure events in a more straightforward
             * manner than relying on read() returning zero,
             * which might only occur after all pending data has been read.
             */
            RdHup = (1 << 2),
        };

        using EventFlags = int;
        using FileDescriptor = int;

        ~IOContext() override;

        std::optional<int> getFD() const;

    protected:
        void update() override;

        // Signals
        //! Signal emitted when an event occurs on any of the file descriptors
        //! connected to this IOContext. The event is a single bit flag
        boost::signals2::signal<void(FileDescriptor, Event)> eventOccur;

    private:
        friend class AbstractSocket;

        explicit IOContext(Node* parent = nullptr);

        bool registerSocketInEPoll(int fd, EventFlags events);

        bool modifySocketEventsInEPoll(int fd, EventFlags events);

        bool unregisterSocketInEPoll(int fd);

    private:
        std::optional<int> m_fd = -1;
        static IOContext* m_instance;
        int m_fd_count = 0;
        std::array<epoll_event, 15> m_events;
    };
} // namespace IRC

#endif // IOCONTEXT_H
