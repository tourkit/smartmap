#include "test.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "node.hpp"


#include "json.hpp"

Test::Test() {

rapidjson::Document document;

Editor<rapidjson::Document>([](Node* node, rapidjson::Document *json){  });

NODE<rapidjson::Document>::onrun([](Node* node, rapidjson::Document *json){  });

NODE<rapidjson::Document>::oncreate([](Node* node, rapidjson::Document *json){  });

auto json_n = engine.tree->addOwnr<rapidjson::Document>();

auto json = json_n->get();

    
if (json->HasMember("test")) PLOGD << "SI";
    
    


}