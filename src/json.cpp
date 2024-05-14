#include "json.hpp"

#include "file.hpp"
#include "log.hpp"
#include "rapidjson/error/en.h"

JSON::JSON() {  }

JSON::JSON(File* file) { load(file); }

JSON::JSON(const char* data) { load(data); }

JSON::~JSON() {  }

bool JSON::load(File* file) {

    this->file = file;

    return load(file->data.data());

}
bool JSON::load(const char* data) {

    loaded = false;

    rapidjson::ParseResult result = document.Parse(data);

    if (!result) PLOGE << rapidjson::GetParseError_En(result.Code()) << " @ " <<  result.Offset();

    if (document.IsNull()) {

        PLOGE << rapidjson::GetParseError_En(document.GetParseError());
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
