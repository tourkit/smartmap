#pragma once
 
#include "../framebuffer.hpp"
#include "../ndi.hpp"
#include "../engine.hpp"
// #include "../thread.hpp"

namespace SmartMap {


struct Output {

    static inline std::vector<Output*> pool;

    std::string name = "SmartMap Output";

    FrameBuffer fb;

    Stack::Cue * cue = nullptr;

    int offset_x, offset_y;

    Output(std::string name, int width, int height);
    virtual ~Output();

    enum TYPE { HDMI, NDI, DMX } type;

    void virtual draw() {}

};


struct VideoOutput : Output {

    VideoOutput(std::string name, int width, int height);
    ~VideoOutput();

};

struct NDIOutput : Output {

    NDI::Sender ndisender;

    // Thread thread;

    std::vector<unsigned char> data;

    NDIOutput(std::string name, int width, int height);
    ~NDIOutput();

};

};
