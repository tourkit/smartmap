

#include "websocket.hpp"
#include <fstream>

#include <uWebSockets/src/App.h>  
struct Websocket::Impl {
    uWS::App app;
};
using namespace uWS;

std::string Websocket::getMime(const std::string &path) {

    if (path.ends_with(".css")) return "text/css";
    if (path.ends_with(".js")) return "application/javascript; charset=utf-8";
    if (path.ends_with(".html")) return "text/html";
    if (path.ends_with(".png")) return "image/png";
    if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return "image/jpeg";
    return "text/plain";

}

void Websocket::addOp(std::string str, std::function<void(const char*)> cb) {

    op_codes.push_back(str);
    op_cbs.push_back(cb);

}

Websocket::Websocket() { impl=  new Impl(); }
Websocket::~Websocket() { delete impl; }

void Websocket::pub(const char* label, const char* data, size_t length) {

    impl->app.getLoop()->defer([&]() {

        static std::string_view view(data, length);
        impl->app.publish("broadcast", view, uWS::OpCode::BINARY);
    });

}

void Websocket::run () {   

        int port = 1337;
        impl->app.get("/*", [](auto *res, auto *req) {

            auto urlv = req->getUrl();

            auto url = std::string(urlv);

            if (url == "/") url = "/index.html";

            std::string path = "." + url;

            std::ifstream file(path, std::ios::binary);

            if (!file.is_open()) {
                res->writeStatus("500 Internal Server Error")
                ->end("index.html not found");
                return;
            }

            std::string mime = getMime(path);

            res->writeHeader("Content-Type", mime);
            std::string content((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
            res->end(content);

        });


        impl->app.ws<std::string>("/*", { 
            .open = [&](auto *ws) {
                std::cout << "Client connected\n";
                
                std::string oplist;

                for (size_t i = 0; i < op_codes.size(); i++)
                    oplist+=std::string(oplist.length()?", ":"")+" "+op_codes[i];
                
                
                ws->send("["+oplist+"]", uWS::OpCode::TEXT);
                ws->subscribe("broadcast");
            },
            .message = [&](auto *ws, std::string_view msg, OpCode op) {

                if (msg.length()<1)
                    return;

                int OP = msg[0];
                
                std::cout << op_codes[OP] << "\n";
                if (OP < op_cbs.size())
                    op_cbs[OP](&msg[1]);

            }

        });
        
    

        impl->app.listen(port, [port](auto *token) {

            if (token) 
                std::cout << "ws://localhost:"<< port << "\n";
            
        });

        impl->app.run();

    }