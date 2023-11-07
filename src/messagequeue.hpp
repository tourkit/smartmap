#pragma once

#include "message.hpp"

#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <list>
#include <map>
#include <memory>
#include <mutex>

class MessageQueue {
public:
    using MessagePtr = std::shared_ptr<Message>;

    using MessageCallback = std::function<void(MessagePtr const&)>;

public: 
    MessageQueue(boost::asio::io_service& iosOut /*, boost::asio::io_service& iosIn */);
    ~MessageQueue();

    void setCallback(MessageType messageType, MessageCallback const& callback);
    void pushMessage(MessagePtr const& message);

private:
    void enableRead();

    boost::process::async_pipe _pipe;
    std::mutex _mutexMessages;
    std::list<MessagePtr> _messages;
    std::map<MessageType, MessageCallback> _readCallbacks;
    char _osefBuffer[64]; // notify read buffer, content is useless
};
