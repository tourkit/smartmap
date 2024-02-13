#include "fps.hpp"

#include <chrono>
#include <thread>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

FPS::FPS(std::string name, float max_fps) : name(name), max_fps(max_fps) { 
    
    pool.push_back(this); 
    
}

float FPS::get() {
    current_time = glfwGetTime();    
    fps = 1./(current_time-last_time);
    return fps;
}

float FPS::run(float max) {

    if (!max) max = max_fps;
    if (max) while ( get() > max+1) std::this_thread::sleep_for(std::chrono::microseconds(10));
    last_time = current_time;
    if (has_dropped) has_dropped = false;
    if (fps<max) has_dropped = true;
    return fps;

}
