#pragma once

#include "member.hpp"

template <typename T>
struct Data : Member {

    T range_from;
    T range_to;
    T default_val;

    template <typename... Args> 
    Data(Args&&... args) : Member(std::forward<Args>(args)...) {

        range_from_ptr = &range_from;
        range_to_ptr = &range_to;
        default_val_ptr = &default_val;

    }

    std::type_index type() override { return typeid(T); }

    uint32_t size() override { return sizeof(T); }

    uint32_t footprint() override { return size(); }
    
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