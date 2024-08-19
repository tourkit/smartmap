#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "struct.hpp"

struct Instance;

struct Buffer : Struct {

    std::vector<char> data;

    static inline int MAX_SIZE = 10000;

    Buffer(std::string name = "buffer_definition");

    Buffer(const Buffer& other);
    
    ~Buffer();

    virtual void upload();

    Instance operator[](std::string name);

    Instance operator[](int id);

    Buffer* copy() override;

    void update() override;

    void printData(int max = 0);

    void remap(Buffer& src_buffer, Member* src_member = nullptr, Member* this_member = nullptr, int src_offset = 0, int this_offset = 0);

    void pre_change() override;

    void post_change(std::vector<NewMember> added = {}) override;

    Buffer* isBuff() override { return this; }

    Instance add(Member* m);

private:

    Buffer* bkp = nullptr;
};
