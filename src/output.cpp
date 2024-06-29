#include "output.hpp"

Output::Output(uint32_t width, uint32_t height, FrameBuffer *fb, uint32_t offset_x, uint32_t offset_y)

    : fb(fb) {

        size(width, height);

        pos(offset_x, offset_y);

}

void Output::size(uint32_t width, uint32_t height) { this->width = width; this->height = height; }

void Output::pos(uint32_t offset_x, uint32_t offset_y) { this->offset_x = offset_x; this->offset_y = offset_y; }
