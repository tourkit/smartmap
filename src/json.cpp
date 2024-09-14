#include "json.hpp"

#include "file.hpp"
#include "log.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include <cstddef>
#include <regex>

JSON::JSON() : JSONVal(document) {  }

JSON::JSON(File* file) : JSON() { load(file); }

JSON::JSON(const char* data) : JSON() { load(data); }

JSON::~JSON() {  }

bool JSON::load(File* file) {

    this->file = file;

    return load(file->data.data());

}




bool JSON::load(const char* data) {

    loaded = false;

    rapidjson::ParseResult result = document.Parse(data);

    if (document.IsNull()) {

        PLOGE << rapidjson::GetParseError_En(result.Code()) << " @ " <<  result.Offset();

        PLOGV << data;

        return false;

    }

    loaded = true;

    return true;

}


void JSON::if_obj_in(std::string name, rapidjson::Value &in, std::function<void(rapidjson::Value::Member&)> cb) {

    if (!in.HasMember(name.c_str()) || !in[name.c_str()].IsObject()) return;

    for (auto &m : in[name.c_str()].GetObj()) cb(m);

}



std::string JSONVal::stringify() {


    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);

    return std::regex_replace(buffer.GetString(), std::regex("[\r\n\t ]+"), " ");

}


  JSONVal::JSONVal(rapidjson::Value &value, std::string name) : value(value), name_v(name) {

    if (value.IsArray()) {

        auto arr =  value.GetArray();

        for (auto &x : arr) childrens.emplace_back(x) ;

        return;
        
    }

    if (value.IsObject()) {

        auto obj =  value.GetObj();

        for (auto &x : obj) 
            if (x.name.IsString()) 
                childrens.emplace_back(x.value, x.name.GetString()) ;

        return;
        
    }

}

JSONVal::~JSONVal() { }

static rapidjson::Value null_val;
static rapidjson::Document null_doc;
static auto null_obj = null_doc.SetObject().GetObject();

JSONVal JSONVal::operator[](std::string name, bool warn) { 


    if (!value.IsObject() || !value.HasMember(name.c_str())) {
        if (warn)
            {PLOGW << "no " << name << " in " << this->name() << " (" << str() << ")";}
        return JSONVal(null_val);
    }
    
    return JSONVal(value[name.c_str()], name);
    
}


JSONVal JSONVal::operator[](int id, bool warn) { 

    if (value.IsArray() && id < value.GetArray().Size()) return JSONVal(value.GetArray()[id]);
    
    if (warn)
        {PLOGW << "no " << id;}

    return JSONVal(null_val);
    
}


std::string JSONVal::name() { return name_v; }

bool JSONVal::isnum() {     
    
    if (value.IsFloat()) return value.GetFloat();

    if (value.IsInt()) return value.GetInt();
    
    return 0; 
}
bool JSONVal::isarr() {     
    
    if (value.IsArray()) return true;
    
    return false; 
}

bool JSONVal::isobj() {     
    
    if (value.IsObject()) return true;
    
    return false; 
}

size_t JSONVal::size() { 

    if (isarr()) return value.GetArray().Size();
    
    if (isobj()) return value.GetObj().MemberCount();
    
    return 0; 
    

}

float JSONVal::num(float def) { 
    
    if (value.IsFloat()) return value.GetFloat();

    if (value.IsInt()) return value.GetInt();
    
    return def; 
    
}

std::string JSONVal::str(std::string def) { 
    
    if (value.IsString()) return value.GetString();  
    
    return def; 
    
}
