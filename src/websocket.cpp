

#include "websocket.hpp"


void WebSocket::run () {


        uWS::App()


    .any("/*", [](auto* res, auto* req) {

        res->writeStatus("200 Found");
        res->end();

        std::string url = std::string(req->getUrl());
        std::string method = std::string(req->getMethod());
        std::transform(method.begin(), method.end(), method.begin(), ::toupper);
        std::string body;

        res->onData([res, method, url, body = std::move(body)](std::string_view chunk, bool isLast) mutable {
            body.append(chunk);

            if (isLast) 
                std::cout << method << " " << url << (strlen(body.c_str()) ? " -> " : "")  << body << std::endl;
                
            
        });
    })



    .ws<std::string>("/*", {

        .message = [this](auto* ws, std::string_view msg, uWS::OpCode op) {
            
          xxx(ws,msg);

        }, 



    })
    .listen(1337, [](auto* token) {
        if (token) {
            std::cout << "WebSocket running on ws://localhost:1337\n";
        }
    })
    .run();

}