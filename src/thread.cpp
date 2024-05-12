#include "thread.hpp"
#include "log.hpp"

#include "messagetype.hpp"

#include <iostream>

Thread::Thread(std::string const& name)
    : _name(name)
    , _ios()
    , _inQueue(_ios)
    , _timer(_ios) {
    _keepRunning = false;
    _timerCalltime = TimePoint::min();
}

Thread::~Thread() {
    assert(!_keepRunning);
    if (_thread.joinable())
        _thread.join();
}

void Thread::start() {
    _keepRunning = true;
    _thread = std::thread([this]() {
        init();
        PLOGV << "init " << _name << " done";
        while (_keepRunning) {
            _ios.run_one();
        }
        deinit();
        PLOGV << "deinit " << _name << " done";
    });
}

void Thread::stop() {
    if (!_keepRunning)
        return;
    _keepRunning = false;
    _inQueue.pushMessage(std::make_shared<Message>(MessageType::WAKE_UP)); // wake up
    if (std::this_thread::get_id() != _thread.get_id()) {
        _thread.join();
    }
}

boost::asio::io_service& Thread::ios() { return _ios; }

MessageQueue& Thread::inQueue() { return _inQueue; }

void Thread::setInQueueCallback(MessageType messageType,
                                MessageQueue::MessageCallback const& callback) {
    _inQueue.setCallback(messageType, callback);
}

void Thread::enableTimer() {
    auto now = Clock::now();
    if (_timerCalltime == TimePoint::min())
        _timerCalltime = now;
    _timerCalltime += _timerPeriod;

    _timer.expires_at(_timerCalltime);
    _timer.async_wait([this](boost::system::error_code const& error) {
        _timerCallback();
        enableTimer();
    });
}
