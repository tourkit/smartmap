#pragma once

#include <cstddef>
#include <cstddef>
#include "vendors/NDI/Include/Processing.NDI.Lib.h"
#include "thread.hpp"
#include "output.hpp"

namespace NDI {

struct Sender : public Output, Thread {

    NDIlib_send_instance_t pNDI_send;
    NDIlib_video_frame_v2_t frame;
    std::vector<uint8_t> currentFrameBuffer;

    Sender(uint32_t width, uint32_t height, std::string name, Layer* layer);
    ~Sender();

    void size(uint32_t width, uint32_t height) override;
    // void pos(uint32_t offset_x, uint32_t offset_y) override;

    void draw() override;

    void init() override; // called from this thread
    void deinit() override; // called from this thread
    void tick();

    // void* data();
    int pixelcount;

    // callable from another thread
    void send();

    std::string name;

    static inline int uid_count = 0;

    int uid = uid_count++;

    std::vector<unsigned char> data;

};

};
