#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <functional>

#include "remap.hpp"



struct Struct;
struct Artnet;

struct Universe {

    std::array<uint8_t,512> data;

    int id;

    std::vector<std::function<void(Universe*)>> callbacks;

    void update();

    struct Remap : ::Remap {

        int channel = 0;
        int offset = 0;

        Remap(void *src, void *dst, Member *s, uint16_t q = 1);

        float get8(uint8_t* data) override;

    };

    std::vector<Remap> remaps;

    Universe(int id = -1);

};
