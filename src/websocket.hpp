
#pragma once

#include <cstdint>

#include <functional>

#include <string>

//#include <memory>

struct Websocket {

    struct Impl; // PIMPL to keep include in cpp with nested struct uWS::App
    Impl* impl;

    Websocket();
    ~Websocket();

    static std::string getMime(const std::string &path);

    std::unordered_map<std::string, uint64_t> OP_CODES = {{"",9}};
    std::vector<std::string> op_codes;
    std::vector<std::function<void(const char*)>> op_cbs;

    void addOp(std::string str, std::function<void(const char*)> cb);

    void run ();

    void pub(const char* label, const char* data, size_t length);

};