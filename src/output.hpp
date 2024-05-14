#pragma once

#include <cstdint>

struct Layer;

struct Output {

    uint32_t width, height, offset_x, offset_y;

    Layer *layer = nullptr;

    Output(uint32_t width, uint32_t height, Layer *layer = nullptr, uint32_t offset_x = 0, uint32_t offset_y = 0);

    virtual void size(uint32_t width, uint32_t height);

    virtual void pos(uint32_t offset_x, uint32_t offset_y);

    virtual void draw() = 0;

};
