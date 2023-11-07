#include "messagequeue.hpp"

MessageQueue::MessageQueue(boost::asio::io_service& iosOut /*, boost::asio::io_service& iosIn */)
    : _pipe(iosOut /*, iosIn*/) {
    enableRead();
}

MessageQueue::~MessageQueue() {}

void MessageQueue::setCallback(MessageType messageType, MessageCallback const& callback) {
    _readCallbacks[messageType] = callback;
}

void MessageQueue::pushMessage(MessagePtr const& message) {
    {
        std::scoped_lock<std::mutex> lock(_mutexMessages);
        _messages.emplace_back(message);
        _pipe.write_some(boost::asio::buffer("1", 1));
    }
}

void MessageQueue::enableRead() {
    _pipe.async_read_some(boost::asio::buffer(_osefBuffer, sizeof(_osefBuffer)),
                          [this](boost::system::error_code const& ec, std::size_t readSize) {
                              (void)readSize;
                              std::list<MessagePtr> messages;
                              {
                                  std::scoped_lock<std::mutex> lock(_mutexMessages);
                                  std::swap(_messages, messages);
                              }
                              for (auto& message : messages) {
                                  auto callbackIt = _readCallbacks.find(message->type);
                                  if (callbackIt != _readCallbacks.end())
                                      callbackIt->second(message);
                              }
                              enableRead();
                          });
}