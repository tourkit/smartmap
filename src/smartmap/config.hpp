#pragma once
#include "../pch.hpp"
#include "../dmx.hpp"

namespace SmartMap {

struct Config {

    DMX::Fixture basic_fixture;

    Config();
    
    void import(std::string filepath); 

};

};
