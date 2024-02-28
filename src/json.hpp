#pragma once


#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"


struct File;

struct JSON {

    rapidjson::Document document;

    File * file = nullptr;

    JSON();

    JSON(File* file);

    ~JSON();

    bool load(File* file);

    static bool exists(rapidjson::Value& source, const char* name);

    static const char* getString(rapidjson::Value& source, const char* name, const char* defaultValue = "UNDEFINED");

    static unsigned int getUint(rapidjson::Value& source, const char* name, unsigned int defaultValue = 0);

    static int getInt(rapidjson::Value& source, const char* name, int defaultValue = 0);

    operator rapidjson::Value&();

    rapidjson::Value& operator[](const char* n);

};