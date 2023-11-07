#pragma once


#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

#include "file.hpp"

struct JSON {

    rapidjson::Document document;

    JSON();
    ~JSON();

    bool load(const char* filename);

    static bool exists(rapidjson::Value& source, const char* name);

    static const char* getString(rapidjson::Value& source, const char* name,
                                 const char* defaultValue = "UNDEFINED");

    static unsigned int getUint(rapidjson::Value& source, const char* name,
                                unsigned int defaultValue = 0);

    static int getInt(rapidjson::Value& source, const char* name, int defaultValue = 0);

    operator rapidjson::Value&();
    File* file = nullptr;

    // struct Data {

    //     rapidjson::Document* doc;
    //     rapidjson::Value * val;


    //     static int getInt(const char* name, int defaultValue = 0) {

    //         return source[name].GetInt();

    //     }

    //     rapidjson::Value& operator[](const char* n) {



    //         if (!val->HasMember(name)) {

    //             std::cout << "member '" << name << "' does not exist" << std::endl;
    //             return;

    //         }
            
    //         val = *(*val)[n];

    //         return *val;

    //     }

    // } data{&document};

    rapidjson::Value& operator[](const char* n);
};