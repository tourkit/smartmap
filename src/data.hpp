#pragma once


#include <typeindex>
#include <boost/type_index.hpp>
#include "member.hpp"

template <typename T>
struct Data : Member {

    T range_from;
    T range_to;
    T default_val;

    Data(std::string name = "") : Member(name) {

        range_from_ptr = &range_from;
        range_to_ptr = &range_to;
        default_val_ptr = &default_val;

        if (std::is_arithmetic<T>::value) {

            memset(&default_val,0,sizeof(T));
            memset(&range_from,0,sizeof(T));
            memset(&range_to,0,sizeof(T));

            // if (typeid(T) == typeid(float) || typeid(T) == typeid(glm::vec2) || typeid(T) == typeid(glm::vec3) || typeid(T) == typeid(glm::vec4) ) *(float*)range_to_ptr = 1.0f;
            // if (typeid(T) == typeid(int)) *(int*)range_to_ptr = 10;
            // if (typeid(T) == typeid(uint32_t)) *(uint32_t*)range_to_ptr = 10;

        }


    }

    Data(std::string name ,T range_from, T range_to, T default_val) : Member(name), range_from(range_from), range_to(range_to), default_val(default_val) {

        range_from_ptr = &range_from;
        range_to_ptr = &range_to;
        default_val_ptr = &default_val;

    }

    std::type_index type() override { return typeid(T); }

    uint32_t size() override { return sizeof(T); }

    bool typed() override { return true; }

    Member* copy(Member* x = nullptr) override {

        if (!x) x = new Data<T>(name_v);

        Member::copy(x);

        ((Data<T>*)x)->range_from = *(T*)range_from_ptr;
        ((Data<T>*)x)->range_to = *(T*)range_to_ptr;
        ((Data<T>*)x)->default_val = *(T*)default_val_ptr;

        return x;

    }


};
