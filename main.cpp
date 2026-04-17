#include "src/timeline.hpp"
#include "src/websocket.hpp"

#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>

std::vector<Timeline> timelines;

std::vector<Timeline>& getTimelines() {
    return timelines;
}

struct Engine {
    
    std::atomic<bool> running{true};

    void update(uint64_t delta) {
        for (auto& t : timelines) {
            t.run(delta);
        }
    }

    std::thread worker;
    void play() {

        worker = std::thread([this]() {

            auto last = std::chrono::high_resolution_clock::now();

            while (running) {

                auto now = std::chrono::high_resolution_clock::now();

                uint64_t delta =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();

                last = now;

                update(delta);

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

    }

};
  
int main() {


    Websocket ws;
    std::vector<char> buffer(10);

    int buf_size = 10;

    std::vector<uint8_t> bufferA(buf_size);
    std::vector<uint8_t> bufferB(buf_size);

    std::atomic<bool> useA{true};
    Engine engine;

    timelines.push_back({});
    auto& t = timelines.back();
std::string test = "tttthisisnotatest";
std::vector<unsigned char> testbuf = {1,2,3,4,5,6,123,254,0};

    t.cb = [&](auto& t){ 
        
    
        auto &buffer = useA.load() ? bufferA : bufferB;

        for (int i = 0; i < buf_size; ++i)
            buffer[i] = static_cast<uint8_t>(t.value() * 255.0f);

        useA.store(!useA.load(std::memory_order_relaxed),
                std::memory_order_release);
                
            const auto &readBuffer =
                useA.load(std::memory_order_acquire) ? bufferB : bufferA;

            std::string payload(
                reinterpret_cast<const char*>(readBuffer.data()),
                readBuffer.size()
            );
        

            ws.pub("broadcast", (const char*)&testbuf[0], 7);          
    };


    //t.cb = [](auto& t){ std::cout << (int)(t.value()*255) << std::endl; };

    t.duration(5000);
    t.looping = true;

    t.sortKeys();

    engine.play();

    int sleep_time = 100;
    int sleep_timeB = 100;

    
     
    
    ws.run();

}