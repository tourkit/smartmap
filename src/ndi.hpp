#pragma once

#include <cstddef>
#include "vendors/NDI/Include/Processing.NDI.Lib.h"
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

    // void* data();
    int size;

    // callable from another thread
    void send();


    std::vector<unsigned char> data;

};

};
