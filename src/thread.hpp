#pragma once

#include "messagequeue.hpp"

#include <boost/asio.hpp>
#include <thread>

class Thread {
public:
    using TimerCallback = std::function<void()>;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

public:
    Thread(std::string const& name);
    virtual ~Thread();

    void start();
    void stop();

    boost::asio::io_service& ios();
    MessageQueue& inQueue();

protected:
    void setInQueueCallback(MessageType messageType, MessageQueue::MessageCallback const& callback);

    template <typename TDuration>
    void setTimerCallback(TDuration const& period, TimerCallback const& cb) {
        _timerPeriod = std::chrono::duration_cast<Duration>(period);
        _timerCallback = cb;
        enableTimer();
    }

private:
    // This init is called in this thread
    virtual void init() = 0;
    // This deinit is called in this thread
    virtual void deinit() = 0;

    void enableTimer();

protected:
    std::string _name;

    // reactor loop
    boost::asio::io_service _ios;

    // thread stuff
    bool _keepRunning;
    std::thread _thread;

    // send messages to thread
    MessageQueue _inQueue;

    TimePoint _timerCalltime;
    Duration _timerPeriod;
    TimerCallback _timerCallback;
    boost::asio::steady_timer _timer;
};