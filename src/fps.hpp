#pragma once

#include <string>
#include <vector>


struct FPS {

    std::string name;

    static inline std::vector<FPS*> pool;

    FPS(std::string name = "FPS", float max_fps = 0);

    float current_time = 0, last_time = 0, fps = 1, max_fps = 0;
    
    bool has_dropped = false;

    float get();
    float run(float max = 0);

    operator float() { return fps; }

};
