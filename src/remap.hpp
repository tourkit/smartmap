#pragma once

#include <cstdint>
#include <vector>
#include <memory>

struct Member;
struct Instance;

struct RemapAGG {

    struct Attribute { float min=0, max=1; bool active = true, skip = false; };// !combining is JUMP member

    Instance* src, * dst;

    char *src_, *dst_;

    std::vector<std::shared_ptr<Attribute>> attributes;

    RemapAGG(Instance* src, Instance* dst);

    virtual void update();

    virtual void run();

};

struct Remap {

    struct Attribute { int combining = 0; float min=0, max=1; bool active = true; };// !combining is JUMP member

    char *src;
    char *dest;
    Member *s;
    uint16_t quantity;

    Remap();

    Remap(void *src, void *dst, Member *s, uint16_t q = 1);

    void attr(std::vector<Attribute> attributes);

    virtual void update();

    virtual float get8(uint8_t* data);
    virtual uint16_t get16(uint8_t* data);
    virtual uint32_t get24(uint8_t* data);
    virtual uint32_t get32(uint8_t* data);

    std::vector<Attribute> attributes;

    void extract(Member* s);

    void reset();

};
