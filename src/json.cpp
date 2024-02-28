#include "json.hpp"

#include "file.hpp"
#include "log.hpp"

JSON::JSON() {}

JSON::JSON(File* file) { load(file); }

JSON::~JSON() {  }

bool JSON::load(File* file) {

    this->file = file;

    document.Parse(file->data.data());

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

JSON::operator rapidjson::Value&() { return document; }

rapidjson::Value& JSON::operator[](const char* n) { return document[n]; }