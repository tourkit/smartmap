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

        JSONVal(rapidjson::Value &value, std::string name = "", JSONVal*owner = nullptr, JSONVal* parent = nullptr);
        JSONVal(const JSONVal& other);
    
        ~JSONVal() ;

        JSONVal& find(std::function<bool(JSONVal&)> cb) ;

        JSONVal& find(std::string name, std::string val);

        JSONVal operator[](std::string name, bool warn = false) ;

        JSONVal operator[](int id, bool warn = false) ;

        bool operator==(const JSONVal& other) const { return &other == this; }

        auto begin() { return childrens.begin(); }

        auto end() { return childrens.end(); }

        std::string name();

        std::string str(std::string def = "");
        float num(float def = 0);
        bool b(bool def = false);
        
        bool isnum();
        bool isarr();
        bool isobj();
        size_t size();

        std::string stringify();

        JSONVal*owner = nullptr, *parent = nullptr;
        
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

static rapidjson::Value null_val;
static rapidjson::Document null_doc;
static auto null_obj = null_doc.SetObject().GetObject();

static JSONVal json_null(null_val);