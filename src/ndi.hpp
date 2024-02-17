#pragma once

#include "include/vendor/NDI/Include/Processing.NDI.Lib.h"
#include "thread.hpp"

namespace NDI {

struct Sender : public Thread {

    NDIlib_send_instance_t pNDI_send;
    NDIlib_video_frame_v2_t frame;
    std::vector<uint8_t> currentFrameBuffer;

    Sender(uint16_t width, uint16_t height);
    ~Sender();

    void init() override; // called from this thread
    void deinit() override; // called from this thread
    void tick();


    // callable from another thread
    void send(unsigned char* data, size_t size);
};

};
