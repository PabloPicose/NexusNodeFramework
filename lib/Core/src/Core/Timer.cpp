#include "Timer.h"
#include <chrono>

namespace IRC {

    Timer::Timer(Node* parent) : Node(parent) {}

    Timer::~Timer() = default;

    bool Timer::isRunning() const { return m_isRunning; }

    void Timer::update() {
        if (!m_isRunning) {
            return;
        }
        const auto now = std::chrono::steady_clock::now();
        if (now >= m_nextActivation) {
            // Execute the timer event
            timeout();
            if (m_isSingleShot) {
                stop();
            } else {
                m_nextActivation += std::chrono::milliseconds(m_interval);
            }
        }
    }

    void Timer::setSingleShot(bool singleShot) { m_isSingleShot = singleShot; }

    bool Timer::isSingleShot() const { return m_isSingleShot; }

    void Timer::setInterval(uint32_t msecs) { m_interval = msecs; }

    uint32_t Timer::getInterval() const { return m_interval; }

    uint32_t Timer::remainingTime() const {
        if (!m_isRunning)
            return 0;
        const auto now = std::chrono::steady_clock::now();
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(m_nextActivation - now).count();
        return (remaining > 0) ? static_cast<uint>(remaining) : 0;
    }

    void Timer::start() {
        m_isRunning = true;
        // constructs the next activation retrieving the current time
        // plus the next time to activate again the timer
        m_nextActivation = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);
    }

    void Timer::stop() { m_isRunning = false; }


    void Timer::singleShot(uint32_t msecs, std::function<void()> callback) {

        Timer* timer = new Timer();
        timer->setInterval(msecs);
        timer->timeout.connect(0, callback);
        timer->timeout.connect(1, [timer]() { timer->deleteLater(); });
        timer->start();
    }
} // namespace IRC
