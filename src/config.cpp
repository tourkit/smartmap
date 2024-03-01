#pragma once

#include "vendors/rapidjson/document.h"
#include "vendors/rapidjson/stringbuffer.h"


#include "file.cpp"
#include "ubo.cpp"


// extern Output* hdmi,* monitor;

struct Config {


    static void load(const char* filename) {
        
        rapidjson::Document json;


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

    static void save() {



    }

};
