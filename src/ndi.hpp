
#pragma once
#include "pch.hpp"
#include "include/vendor/NDI/Include/Processing.NDI.Lib.h"

namespace NDI {

    struct Sender {

        NDIlib_send_instance_t pNDI_send;
        NDIlib_video_frame_v2_t* NDI_video_frame;

        Sender(uint16_t width, uint16_t height) {

            NDIlib_send_create_t send_create;

            send_create.p_ndi_name = "Smartmap Output";
            send_create.p_groups = NULL;
            send_create.clock_video = true;

            pNDI_send = NDIlib_send_create(&send_create);
            if (!pNDI_send) std::cout << "NDI ERROR 1" << std::endl;

            
            NDI_video_frame = new NDIlib_video_frame_v2_t;
            NDI_video_frame->xres = width;
            NDI_video_frame->yres = height;
            NDI_video_frame->FourCC = NDIlib_FourCC_type_BGRA;

            // NDI_video_frame->frame_rate_N = 30000;
            // NDI_video_frame->frame_rate_D = 1001;
            // NDI_video_frame->picture_aspect_ratio = 16.0f/9.0f;
            // NDI_video_frame->frame_format_type = NDIlib_frame_format_type_progressive;
            // NDI_video_frame->timecode = 0;
            // NDI_video_frame->line_stride_in_bytes = 1920*4;
            // NDI_video_frame->p_metadata = "<Hello/>";

            NDI_video_frame->p_data = (uint8_t*)malloc(NDI_video_frame->xres * NDI_video_frame->yres * 4);

            memset((void*)NDI_video_frame->p_data, 0 , NDI_video_frame->xres * NDI_video_frame->yres * 4);

            
        }

        void send(unsigned char* data, size_t size) {

            memcpy(NDI_video_frame->p_data, data, size);

            send();

        }

        void send() { NDIlib_send_send_video_async_v2(pNDI_send, NDI_video_frame); }

    };

};