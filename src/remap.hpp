#pragma once

#include <cstdint>

// namespace DMX { struct Fixture; };

struct Struct;

struct Remap { 

    char *src;  
    char *dest;  
    Struct *s;
    uint16_t quantity;

    Remap(){};
    
    Remap(void *src, void *dst, Struct *s, uint16_t q = 1) : src((char*)src), dest((char*)dst), s(s), quantity(q) { update(); }

    virtual void update();

    virtual uint8_t get8(uint8_t* data);
    virtual uint16_t get16(uint8_t* data);
    virtual uint32_t get24(uint8_t* data);
    virtual uint32_t get32(uint8_t* data);
    
};