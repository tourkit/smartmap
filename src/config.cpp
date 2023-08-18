#pragma once

#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

#include "pch.cpp"

#include "file.cpp"


// extern Output* hdmi,* monitor;

struct Config {

    rapidjson::Document json;

    std::string ip;
    
    uint8_t subnet;

    uint16_t width, height;

    Config(const char* filename) {

        File file(filename);
        json.Parse(file->data.data());

        if (json.HasMember("ip")) ip = json["ip"].GetString();
        else ip = "10.0.0.99";

        if (json.HasMember("subnet")) subnet = json["subnet"].GetUint();
        else subnet = 24;

        // width = JSON::getUint(json["outputs"][0], "width", 255);
        // height = JSON::getUint(json["outputs"][0], "height", 255);

        /* temporary bullshit cote */
        if (json.HasMember("outputs") && json["outputs"].Size()) {

            // forin

        }

    }

    int count = 0;

    std::map<std::string, Output*> outputs;
    std::vector<Fixture*> fixtures;


    void addGridLayer(int columns, int rows, int address, int universe, float width, float height, float offset_x, float offset_y, FixtureMode fixture_mode, const char *layer_mode, Output* output);
    int addFreeLayer(int quantity, int address, int universe, float width, float height, float offset_x, float offset_y, FixtureMode fixture_mode, const char *layer_mode, Output* output);
    int addFixture(int address, int universe, float width, float height, float offset_x, float offset_y, FixtureMode fixture_mode, const char *layer_mode, Output* output);

    void createShow();

};
