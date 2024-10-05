#pragma once

#include <Core/Node.h>

#include <chrono>

namespace IRC {
    class Timer : public Node {
    public:
        explicit Timer(Node* parent = nullptr);

        ~Timer() override;

        void update() override;

        bool isRunning() const;

        void setSingleShot(bool singleShot);

        bool isSingleShot() const;

        void setInterval(uint32_t msecs);

        uint32_t getInterval() const;

        uint32_t remainingTime() const;

        void start();

        void stop();

        boost::signals2::signal<void()> timeout;

        static void singleShot(uint32_t msecs, std::function<void()> callback);


    private:
        bool m_isSingleShot = true;
        bool m_isRunning = false;
        uint32_t m_interval = 0;
        std::chrono::steady_clock::time_point m_nextActivation;
    };
} // namespace IRC
