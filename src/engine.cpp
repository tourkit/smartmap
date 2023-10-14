#include "engine.hpp"

Engine::Engine(uint16_t width, uint16_t height) 

    : window(width,height), 
    dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"), 
    quad("quad.obj",width,height, "quad"),
    fb(0,width,height,"mainFB"), gui(window.id) {

}