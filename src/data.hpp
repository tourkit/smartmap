#pragma once


#include <typeindex>
#include <boost/type_index.hpp>
#include "member.hpp"
#include "log.hpp"

template <typename T>
struct Data : Member {

    T range_from;
    T range_to;
    T default_val;

    Data(std::string name = "", uint32_t quantity = 1) : Member(name) {

        this->quantity_v = (quantity);

        range_from_ptr = &range_from;
        range_to_ptr = &range_to;
        default_val_ptr = &default_val;

        memset(&default_val,0,sizeof(T));
        memset(&range_from,0,sizeof(T));
        memset(&range_to,0,sizeof(T));

        if (typeid(float) == typeid(T))   *(float*)range_to_ptr = 1.0f;

        else if (typeid(uint32_t) == typeid(T))   *(uint32_t*)range_to_ptr = 65535;

        else  if (typeid(T) == typeid(float) || typeid(T) == typeid(glm::vec2) || typeid(T) == typeid(glm::vec3) || typeid(T) == typeid(glm::vec4) ) *(float*)range_to_ptr = 1.0f;

        else if (typeid(char) == typeid(T))   *(char*)range_to_ptr = 255;

    }

    Data(std::string name ,T range_from, T range_to, T default_val) : Member(name), range_from(range_from), range_to(range_to), default_val(default_val) {

        range_from_ptr = &range_from;
        range_to_ptr = &range_to;
        default_val_ptr = &default_val;

    }

    Data(const Data& other) : Member(other), range_from(other.range_from), range_to(other.range_to), default_val(other.default_val) {

    }


    ~Data() { }

    std::type_index type() override { return typeid(T); }

    uint32_t size() override { return sizeof(T); }

    bool isData() override { return true; }

    Member* copy() override { return new Data<T>(*this); }

};
