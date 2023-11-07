#include "config.hpp"  
#include "layer.hpp"  
#include "../smartmap.hpp"  

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
  
    json.Parse(File(filepath).data.data());

    if(json.HasParseError()) { std::cout << "SM config json parse error !" << std::endl; return; }

    ip = JSON::getString(json, "ip", "10.0.0.99");

    subnet = JSON::getUint(json, "subnet", 24);

    if (!json.HasMember("outputs")) { std::cout << "Missing outputs" << std::endl; return; }
    if (!json["outputs"].Size()) { std::cout << "Empty outputs" << std::endl; return; }
    for (auto& output : json["outputs"].GetArray()) {

        // 
        const char* name = JSON::getString(output, "name");
        unsigned int width = JSON::getUint(output, "width", 16);
        unsigned int height = JSON::getUint(output, "height", 16);

        auto* type = JSON::getString(output, "type");


        std::cout << "new \"" << name << "\" output \n";

        if (!strcmp(type, "HDMI")) {
            
            outputs[name] = new VideoOutput(name,width,height);

            continue;
        }

        if (!strcmp(type, "NDI")) {

            outputs[name] = new NDIOutput(name,width, height);
            outputs[name]->name = name;

            continue;

        }

        std::cout << "could not create \"" << name << "\" output ... \n";

    }

    if (!Output::pool.size()) {std::cout << "No outputs ... \n"; return; }

    if (!json.HasMember("layers")) { std::cout << "Missing layers" << std::endl; return; }
    if (!json["layers"].Size()) { std::cout << "Empty layers" << std::endl; return; }
    for (auto& layer : json["layers"].GetArray()) {

        auto* name = JSON::getString(layer, "name");

        int output_id = 0;
        auto layer_name = JSON::getString(layer, "output");
        for (auto o:Output::pool) { if (!strcmp(o->name.c_str(),layer_name)) { break;} output_id++; }
        if (output_id == Output::pool.size()) { 

            std::cout << "layer " << name << " : invalid output !" << std::endl;
            continue;
            // output_id = 0; 

        }

        auto* output =  Output::pool[output_id];

        auto startUniverse = JSON::getUint(layer, "start_universe", 1);
        auto startAddress = JSON::getUint(layer, "start_address", 1);

        // wtf ?
        if (startAddress == 0 || startAddress > 512) // devrait pas etre 511 ?
            startAddress = 0;
        else
            --startAddress;

        if (startUniverse == 0)
            startUniverse = -1; // ArtNet::PHYSICAL_DMXIN_UNIVERSE;
        else if (startUniverse > 0x8000)
            startUniverse = 0;
        else
            --startUniverse;

        auto offset_x = JSON::getUint(layer, "x_offset", 0);
        auto offset_y = JSON::getUint(layer, "y_offset", 0);

        auto width = JSON::getUint(layer, "width", 0);
        auto height = JSON::getUint(layer, "height", 0);

        if (!width)
            width = output->fb.width;
        if (!height)
            height = output->fb.height;

        auto* t_fixture_mode = JSON::getString(layer, "fixture_mode");

        // FixtureMode fixture_mode = FixtureMode::Basic;

        // if (!strcmp(t_fixture_mode, "Player"))
        //     fixture_mode = FixtureMode::Player;
        // else if (!strcmp(t_fixture_mode, "Basic"))
        //     fixture_mode = FixtureMode::Basic;
        // else if (!strcmp(t_fixture_mode, "Extended"))
        //     fixture_mode = FixtureMode::Extended;

        auto columns = JSON::getUint(layer, "columns", 1);
        auto rows = JSON::getUint(layer, "rows", 1);
        auto quantity = JSON::getUint(layer, "quantity", 0);
        if (quantity) columns = quantity;
    
        auto* layer_mode = JSON::getString(layer, "layer_mode");
        Layer::Mode mode = Layer::Mode::Free;
        if (!strcmp(layer_mode, "Grid")) mode = Layer::Mode::Grid;
        std::cout << "mode: " << mode << std::endl;
        
        new Layer(

            startAddress, 
            startUniverse, 
            basic_fixture, 
            engine.window.width,
            engine.window.height, 
            mode, 
            columns,
            rows,
            1,
            output_id

        );

        }

    }

};