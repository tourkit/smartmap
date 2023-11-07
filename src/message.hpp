#ifndef __TOURKIT_MESSAGE_HPP__
#define __TOURKIT_MESSAGE_HPP__

enum class MessageType;

class Message {
public:
    MessageType type;
    Message(MessageType t)
        : type(t) {}
    virtual ~Message() {}
};

#endif