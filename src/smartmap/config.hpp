#pragma once
#include "../pch.hpp"
#include "../dmx.hpp"
#include "../json.hpp"
#include "output.hpp"

namespace SmartMap {

struct Config {

    rapidjson::Document json;

    DMX::Fixture basic_fixture;

    std::string ip;
    
    unsigned int subnet;

    std::map<std::string, Output*> outputs;

    Config();
    
    void import(std::string filepath); 

};

};
