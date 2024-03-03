#pragma once


#include "vendors/rapidjson/document.h"
#include "vendors/rapidjson/stringbuffer.h"
#include "../vendors/rapidjson/prettywriter.h"

#include "log.hpp"

#include <functional>

struct File;

struct JSON {

    rapidjson::Document document;

    File * file = nullptr;

    bool loaded;

    JSON();

    JSON(File* file);
    
    JSON(const char* data);

    ~JSON();

    bool load(File* file);

    bool load(const char* data);

    static bool exists(rapidjson::Value& source, const char* name);

    static const char* getString(rapidjson::Value& source, const char* name, const char* defaultValue = "UNDEFINED");

    static unsigned int getUint(rapidjson::Value& source, const char* name, unsigned int defaultValue = 0);

    static int getInt(rapidjson::Value& source, const char* name, int defaultValue = 0);

    rapidjson::GenericObject<false, rapidjson::Value> operator[](const char* name) { return obj(name); }

    rapidjson::GenericObject<false, rapidjson::Value> obj(const char* name) {

         if (!document.HasMember(name) || !document[name].IsObject()) { 

            PLOGW << "UNDEFINED: " << name;

            rapidjson::Value models;

            models.SetObject(); 

            document.AddMember("models", models, document.GetAllocator());
            
            
        }

        return document[name].GetObj();

     }

};