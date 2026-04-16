#include "uWebSockets/src/App.h"
#include <atomic>
#include <cmath>
#include <vector>
#include <chrono>
#include <functional>

using Clock = std::chrono::steady_clock;

// ==============================
// EASING FUNCTIONS
// ==============================

namespace Easing {
    inline float linear(float t) {
        return t;
    }

    inline float easeInOut(float t) {
        return t * t * (3.0f - 2.0f * t); // smoothstep
    }

    inline float easeIn(float t) {
        return t * t;
    }

    inline float easeOut(float t) {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }
}

// ==============================
// CUSTOM CURVE (interpolation)
// ==============================

class Curve {
public:
    std::vector<std::pair<float, float>> points;

    float sample(float t) const {
        if (points.empty()) return t;

        for (size_t i = 1; i < points.size(); ++i) {
            if (t <= points[i].first) {
                auto [t0, v0] = points[i - 1];
                auto [t1, v1] = points[i];

                float alpha = (t - t0) / (t1 - t0);
                return v0 + alpha * (v1 - v0);
            }
        }
        return points.back().second;
    }
};

// ==============================
// OSCILLATOR (LOCK-FREE)
// ==============================

class Oscillator {
public:
    // paramètres atomiques (lock-free)
    std::atomic<float> amplitude {50.0f};
    std::atomic<float> offset {50.0f};      // centre
    std::atomic<float> frequency {1.0f};    // cycles/sec
    std::atomic<float> phaseOffset {0.0f};  // décalage

    std::function<float(float)> easing = Easing::easeInOut;
    Curve curve;

    Oscillator() {
        startTime = Clock::now();
    }

    float getValue() const {
        auto now = Clock::now();
        float timeSec = std::chrono::duration<float>(now - startTime).count();

        float freq = frequency.load(std::memory_order_relaxed);
        float phase = phaseOffset.load(std::memory_order_relaxed);

        float t = std::fmod(timeSec * freq + phase, 1.0f);

        // appliquer easing ou courbe custom
        float shaped = curve.points.empty() ? easing(t) : curve.sample(t);

        float amp = amplitude.load(std::memory_order_relaxed);
        float off = offset.load(std::memory_order_relaxed);

        return off + (shaped * 2.0f - 1.0f) * amp;
    }

    float sample(float tGlobal) const {
        float f = frequency.load(std::memory_order_relaxed);
        float p = phaseOffset.load(std::memory_order_relaxed);

        float t = std::fmod(tGlobal * f + p, 1.0f);

        float v = 0.5f + 0.5f * std::sin(t * 2.0f * M_PI);

        float a = amplitude.load(std::memory_order_relaxed);
        float o = offset.load(std::memory_order_relaxed);

        return o + (v * 2.0f - 1.0f) * a;
    }
private:
    Clock::time_point startTime;
};


std::string loadFile(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
std::string getMime(const std::string &path) {
    if (path.ends_with(".css")) return "text/css";
    if (path.ends_with(".js")) return "application/javascript; charset=utf-8";
    if (path.ends_with(".html")) return "text/html";
    if (path.ends_with(".png")) return "image/png";
    if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return "image/jpeg";
    return "text/plain";
}

    std::unordered_map<std::string, uint64_t> OP_CODES = {{"",9}};
    std::vector<std::string> op_codes;
    std::vector<std::function<void(const char*)>> op_cbs;

    void addOp(std::string str, std::function<void(const char*)> cb) {

        op_codes.push_back(str);
        op_cbs.push_back(cb);

    }



int main() {


    int sleep_time = 100;
    int sleep_timeB = 100;

    addOp("sleep_time", [&](auto data){ sleep_time = *(int*)data; /* bimbamatacksurface */ });
    addOp("sleep_timeB", [&](auto data){ sleep_timeB = *(int*)data; });

    
    // buffer fixe (pas de realloc)
    std::vector<char> buffer(10);

    int buf_size = 10;

    std::vector<uint8_t> bufferA(buf_size);
    std::vector<uint8_t> bufferB(buf_size);

    std::atomic<bool> useA{true};
    
    auto start = Clock::now();



    // 32 oscillateurs
    std::vector<Oscillator> oscillators(32);

    for (int i = 0; i < 32; ++i) {
        oscillators[i].frequency = 0.1f + i * 0.05f;
        oscillators[i].phaseOffset = i * 0.13f;
        oscillators[i].amplitude = 20.0f + (i % 6) * 8.0f;
    }
    using namespace uWS;

    App app;

    int port = 1337;
    app.get("/*", [](auto *res, auto *req) {

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


    app.ws<std::string>("/*", {
        .open = [](auto *ws) {
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
    

    std::thread producer([&]() {

        while (true) {

            float t = std::chrono::duration<float>(Clock::now().time_since_epoch()).count();

            auto &buffer = useA.load() ? bufferA : bufferB;

            for (int i = 0; i < buf_size; ++i)
                buffer[i] = static_cast<uint8_t>(oscillators[i % 32].getValue() * 255.0f);

            useA.store(!useA.load(std::memory_order_relaxed),
                    std::memory_order_release);

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

        }

    });
    
    std::thread broadcaster([&] {

        while (true) {

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_timeB));

            const auto &readBuffer =
                useA.load(std::memory_order_acquire) ? bufferB : bufferA;

            std::string payload(
                reinterpret_cast<const char*>(readBuffer.data()),
                readBuffer.size()
            );

            app.getLoop()->defer([&]() {
                app.publish("broadcast", payload, uWS::OpCode::BINARY);
            });
        }
    });

    app.listen(port, [port](auto *token) {

        if (token) 
            std::cout << "ws://localhost:"<< port << "\n";
        
    });

    app.run();

}



