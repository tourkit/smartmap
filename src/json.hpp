#pragma once


#include "vendors/rapidjson/document.h"
#include "vendors/rapidjson/stringbuffer.h"
#include "../vendors/rapidjson/prettywriter.h"

#include "log.hpp"

#include <functional>

struct File;

    struct JSONVal { 
        
        rapidjson::Value &value; 

        std::string name_v;

        std::vector<JSONVal> childrens; 

        JSONVal(rapidjson::Value &value, std::string name = "");

        ~JSONVal() ;


        JSONVal operator[](std::string name) ;


        JSONVal operator[](int id) ;
        

        auto begin() { 
            
            return childrens.begin();
            
        }

        auto end() { 
    
            return childrens.end();
            
        }

        std::string name();

        std::string str();
        
        float num();
        
    };
struct JSON : JSONVal {

    rapidjson::Document document;

    File * file = nullptr;

    bool loaded = false;

    JSON();

    JSON(File* file);

    JSON(const char* data);

    ~JSON();

    bool load(File* file);

    bool load(const char* data);

    static void if_obj_in(std::string name, rapidjson::Value &in, std::function<void(rapidjson::Value::Member&)> cb);

    
};
