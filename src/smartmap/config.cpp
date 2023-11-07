#include "config.hpp"  
#include "layer.hpp"  
#include "../smartmap.hpp"  

#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

namespace SmartMap {
 
Config::Config () {

       basic_fixture.presets = { 

        {"DIMMER", {
            {"DIMMER", {
                {"Dim",         1,  &Component::id("Opacity")->members[0]},
        }}}},

        {"COLOR", {
            {"RGB", {
                {"Red",         1,  &Component::id("RGB")->members[0]}, 
                {"Green",       1,  &Component::id("RGB")->members[1]}, 
                {"Blue",        1,  &Component::id("RGB")->members[2]},                            
        }}}},

        {"Position", {
            {"Position", { 
                {"Pos_X",       2,  &Component::id("Position")->members[0]}, 
                {"Pos_Y",       2,  &Component::id("Position")->members[1]},                                      
        }}}},

        {"Focus", {
            {"Focus", {
                {"Focus_X",     2,  &Component::id("Size")->members[0]}, 
                {"Focus_Y",     2,  &Component::id("Size")->members[1]},                                  
        }}}},

        {"Gobo", {
            {"Gobo", {
                {"Gobo_ID",     1,  &Component::id("Gobo")->members[0]}, 
                {"Gobo_FX1",    1,  &Component::id("Gobo")->members[1]}, 
                {"Gobo_FX2",    1,  &Component::id("Gobo")->members[2]}, 
                {"Gobo_FX3",    1,  &Component::id("Gobo")->members[3]},
        }}}},

        {"Beam", {
            {"Beam", {
                {"Orientation", 2,  &Component::id("Orientation")->members[0]}, 
                {"Feedback",    1,  &Component::id("Feedback")->members[0]}, 
                {"Strobe",      1,  &Component::id("Strobe")->members[0]},               
        }}}},

    };


}

void Config::import(std::string filepath) {


    rapidjson::Document json;
    json.Parse(File(filepath).data.data());
    if(json.HasParseError()) { std::cout << "SM config json parse error !" << std::endl; return; }

    if (!json.HasMember("layers")) { std::cout << "no layers in config" << std::endl; return; }

    for (auto &layer : json["layers"].GetArray()) { 

        SmartMap::Layer::Mode mode = SmartMap::Layer::Mode::Free;
        if (!strcmp(layer["mode"].GetString(),"Grid")) mode = SmartMap::Layer::Mode::Grid;

        int columns = 1, rows = 1;
        if (layer.HasMember("columns")) columns = layer["columns"].GetInt();
        if (layer.HasMember("rows")) rows = layer["rows"].GetInt();
        if (layer.HasMember("quantity")) columns = layer["rows"].GetInt();

        new SmartMap::Layer(

            layer["start_channel"].GetInt(), 
            layer["start_universe"].GetInt(), 
            basic_fixture, 
            engine.window.width,
            engine.window.height, 
            mode, 
            columns,
            rows,
            1,
            layer["output"].GetInt()


        );
    
    }
    
}

};