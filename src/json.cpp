#include "json.hpp"

#include "file.hpp"
#include "log.hpp"
#include "rapidjson/error/en.h"

JSON::JSON() {}

JSON::JSON(File* file) { load(file); }

JSON::JSON(const char* data) { load(data); }

JSON::~JSON() {  }

bool JSON::load(File* file) {

    this->file = file;

    return load(file->data.data());

}
bool JSON::load(const char* data) {

    loaded = false;

    document.Parse(data);

    if (document.IsNull()) { 
        
        PLOGW << rapidjson::GetParseError_En(document.GetParseError()); 

        document.Parse("{}");
        
        return false; 
        
    }

    loaded = true;
    
    return true;

}

bool JSON::exists(rapidjson::Value& source, const char* name) {

    if (!source.HasMember(name)) {

        PLOGW << "-----------------------------> undefined: " << name;

        return false;
    }

    return true;
}

const char* JSON::getString(rapidjson::Value& source, const char* name, const char* defaultValue) {

    if (!exists(source, name))
        return defaultValue;

    return source[name].GetString();
}

unsigned int JSON::getUint(rapidjson::Value& source, const char* name, unsigned int defaultValue) {

    if (!exists(source, name))
        return defaultValue;

    return source[name].GetUint();
}

int JSON::getInt(rapidjson::Value& source, const char* name, int defaultValue) {

    if (!exists(source, name))
        return defaultValue;

    return source[name].GetInt();
}
