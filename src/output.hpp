#pragma once

#include <cstdint>

struct FrameBuffer;

struct Output {

    uint32_t width, height;
    int offset_x, offset_y;

    FrameBuffer *fb = nullptr;

    Output(uint32_t width, uint32_t height, FrameBuffer *fb = nullptr, int offset_x = 0, int offset_y = 0);

    virtual void size(uint32_t width, uint32_t height);

    virtual void pos(int offset_x, int offset_y);

    virtual void draw() = 0;

};
