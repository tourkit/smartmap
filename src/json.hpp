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

    bool loaded = false;

    JSON();

    JSON(File* file);

    JSON(const char* data);

    ~JSON();

    bool load(File* file);

    bool load(const char* data);

    static void if_obj_in(std::string name, rapidjson::Value &in, std::function<void(rapidjson::Value::Member&)> cb);
};
