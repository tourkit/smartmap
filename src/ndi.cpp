#include "ndi.hpp"
#include "log.hpp"

#include "messagetype.hpp"

namespace NDI {

class FrameOutMessage : public Message {
public:
    FrameOutMessage()
        : Message(MessageType::NDI_FRAME_OUT) {}

    std::vector<uint8_t> data;
};

Sender::Sender(uint16_t width, uint16_t height) : Thread("NDI::Sender") {
    frame.xres = width;
    frame.yres = height;
    frame.FourCC = NDIlib_FourCC_type_BGRA;

    // frame.frame_rate_N = 30000;
    // frame.frame_rate_D = 1001;
    // frame.picture_aspect_ratio = 16.0f/9.0f;
    // frame.frame_format_type = NDIlib_frame_format_type_progressive;
    // frame.timecode = 0;
    // frame.line_stride_in_bytes = 1920*4;
    // frame.p_metadata = "<Hello/>";
}

Sender::~Sender() {}

void Sender::init() {
    NDIlib_send_create_t send_create;

    send_create.p_ndi_name = "Smartmap Output";
    send_create.p_groups = NULL;
    send_create.clock_video = true;

    pNDI_send = NDIlib_send_create(&send_create);
    if (!pNDI_send) PLOGE << "NDI ERROR 1" ;

    setInQueueCallback(MessageType::NDI_FRAME_OUT, [this](std::shared_ptr<Message> const& m) {
        auto message = std::static_pointer_cast<FrameOutMessage>(m);
        currentFrameBuffer.swap(message->data);
    });

    setTimerCallback(std::chrono::microseconds(1000000/30), [this]() {
        tick();
    });
}

void Sender::deinit() {
    NDIlib_send_destroy(pNDI_send);
}

void Sender::tick() {
    if (currentFrameBuffer.size()) {
        frame.p_data = currentFrameBuffer.data();
        NDIlib_send_send_video_v2(pNDI_send, &frame);
        currentFrameBuffer.resize(0);
    }
}

void Sender::send(unsigned char* data, size_t size) {
    auto message = std::make_shared<FrameOutMessage>();
    message->data.insert(message->data.begin(), data, data + size);
    inQueue().pushMessage(message);
}

}
