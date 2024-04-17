#pragma once

#include <cstdint>
#include <vector>

struct Member;

struct Remap {

    char *src;
    char *dest;
    Member *s;
    uint16_t quantity;

    Remap();

    Remap(void *src, void *dst, Member *s, uint16_t q = 1);

    virtual void update();

    virtual uint8_t get8(uint8_t* data);
    virtual uint16_t get16(uint8_t* data);
    virtual uint32_t get24(uint8_t* data);
    virtual uint32_t get32(uint8_t* data);

    struct Attribute { int combining = 0; float min=0, max=1; };// !combining is JUMP member

    std::vector<Attribute> attributes;

    void extract(Member* s);

};
