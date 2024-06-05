//
// Created by pablo on 5/29/24.
//

#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include "Core/Node.h"

namespace IRC {
    class IOContext;

    class AbstractSocket : public Node {
    public:
        ~AbstractSocket() override;

        /**
         * @brief Closes the socket. If the socket is already closed or the file descriptor
         * is not initialiced, this function does nothing.
         * @details This function closes the socket and emits the socketClosed signal.
         */
        virtual void close();

        std::optional<int> getFileDescriptor() const;

        IOContext* getIOContext() const;

        /**
         * This signal is emitted when the socket is closed and the file descriptor is no longer valid.
         * Delete the socket inside this function is not recommended, instead use the deleteLater() function.
         */
        boost::signals2::signal<void(AbstractSocket* emitter)> socketClosed;

    protected:
        explicit AbstractSocket(Node* parent = nullptr);

        virtual bool setFileDescriptor(std::optional<int> fd);

        bool setNonBlocking(int fd);

        boost::signals2::signal<void()> inEvent;
        boost::signals2::signal<void()> outEvent;
        boost::signals2::signal<void()> rdhupEvent;

        //! This function is implemented to avoid the required update implementation
        //! in the derived classes. This function does nothing and it's not required
        //! to be called.
        void update() override;

        bool registerSocket(int fd, int events);

        bool modifySocket(int fd, int events);

        bool unregisterSocket(int fd);

        //! Returns the flags associated with the file descriptor (Linux flags)
        int getFlags() const;

    private:
        // slots
        void onEventOcur(int fd, int event);

    private:
        std::optional<int> m_fd;
    };
}


#endif //ABSTRACTSOCKET_H
