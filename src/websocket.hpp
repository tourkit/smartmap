
#pragma once
#include <uWebSockets/src/App.h>
#include <functional>

struct WebSocket {


    std::function<void(uWS::WebSocket<false, true, std::basic_string<char>>*, std::string_view)> xxx;


    void run ();


};