

#include "websocket.hpp"
#include "timeline.hpp"
#include <fstream>

#include <uWebSockets/src/App.h>

std::vector<Timeline>& getTimelines();

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

static std::string serializeTimelines() {
    auto& tls = getTimelines();
    std::string ss = "[";
    for (size_t i = 0; i < tls.size(); ++i) {
        if (i > 0) ss += ",";
        auto& t = tls[i];
        ss += "{\"id\":" + std::to_string(i) + ",\"duration\":" + std::to_string(t.duration_v) 
           + ",\"looping\":" + (t.looping ? "true" : "false")
           + ",\"current\":" + std::to_string(t.current) + ",\"keys\":[";
        for (size_t j = 0; j < t.keys.size(); ++j) {
            if (j > 0) ss += ",";
            auto& k = t.keys[j];
            ss += "{\"position\":" + std::to_string(k.position) + ",\"value\":" + std::to_string(k.value) 
               + ",\"type\":" + std::to_string(k.type) + "}";
        }
        ss += "]}";
    }
    ss += "]";
    return ss;
}

void Websocket::run () {   

        int port = 1337;
        
        impl->app.get("/api/timelines", [](auto *res, auto *req) {
            auto urlv = req->getUrl();
            std::string url(urlv);
            
            size_t qpos = url.find('?');
            std::string path = qpos == std::string::npos ? url : url.substr(0, qpos);
            std::string query = qpos == std::string::npos ? "" : url.substr(qpos + 1);
            
            if (path == "/api/timelines" && !query.empty()) {
                try {
                    auto& tls = getTimelines();
                    
                    std::map<std::string, std::string> params;
                    size_t start = 0;
                    while (start < query.size()) {
                        size_t eq = query.find('=', start);
                        size_t amp = query.find('&', start);
                        if (eq == std::string::npos) break;
                        std::string key = query.substr(start, eq - start);
                        std::string val = (amp == std::string::npos) ? 
                            query.substr(eq + 1) : query.substr(eq + 1, amp - eq - 1);
                        params[key] = val;
                        start = (amp == std::string::npos) ? query.size() : amp + 1;
                    }
                    
                    auto it = params.find("action");
                    if (it != params.end()) {
                        std::string action = it->second;
                        
                        if (action == "create") {
                            uint64_t duration = 5000;
                            bool looping = false;
                            if (params.count("duration")) duration = std::stoull(params["duration"]);
                            if (params.count("looping")) looping = (params["looping"] == "true");
                            tls.push_back(Timeline(duration));
                            tls.back().looping = looping;
                        }
                        else if (action == "update" && params.count("id")) {
                            size_t id = std::stoul(params["id"]);
                            if (id < tls.size()) {
                                auto& t = tls[id];
                                if (params.count("duration")) t.duration(std::stoull(params["duration"]));
                                if (params.count("looping")) t.looping = (params["looping"] == "true");
                                if (params.count("current")) t.current = std::stoull(params["current"]);
                                if (params.count("keys")) {
                                    // keys are JSON encoded - need a proper parser
                                }
                            }
                        }
                        else if (action == "delete" && params.count("id")) {
                            size_t id = std::stoul(params["id"]);
                            if (id < tls.size()) tls.erase(tls.begin() + id);
                        }
                    }
                    
                    res->writeHeader("Content-Type", "application/json");
                    res->end(serializeTimelines());
                } catch (std::exception& e) {
                    res->writeStatus("400 Bad Request");
                    res->end(std::string("Error: ") + e.what());
                }
            } else {
                res->writeHeader("Content-Type", "application/json");
                res->end(serializeTimelines());
            }
        });

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