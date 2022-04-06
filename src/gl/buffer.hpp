#ifndef BUFFER_HPP
#define BUFFER_HPP


#include <GL/gl3w.h>

#include "file.hpp"

// #include "smartmap/enums.hpp"

namespace Engine {

struct Buffer {

    static inline int current_slot = 1;

    GLuint tex_id, fb_id;

    int slot_id = 0;

    int width, height;

    bool initialized = false;

    Buffer(GLuint width, GLuint height) : width { 
    
        glGenTextures(1, &tex_id);

        setMode(0);

        glGenFramebuffers(1, &fb_id);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);

        send(width,height,nullptr);

    
    }

    void setMode(int id) {

        glBindTexture(GL_TEXTURE_2D, tex_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // AQUI STA REPEATOUM
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    }

    ~Buffer() {

        glDeleteTextures(1, &tex_id);

        glDeleteFramebuffers(1, &fb_id);

    }

    int slot() {

        if (!slot_id) slot_id = current_slot++;

        glActiveTexture(GL_TEXTURE0+slot_id);

        glBindTexture(GL_TEXTURE_2D, tex_id);

        glActiveTexture(GL_TEXTURE0);

        return slot_id;

    }

    void bind() {

        glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

        glViewport(0, 0, width, height);
    }

    void clear(float r = 0, float g = 0, float b = 0, float a = 1) {

        bind();

        glClearColor(r,g,b,a); // BG COLOR

        glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    }

    void send(GLuint width, GLuint height, void* data) {

        glBindTexture(GL_TEXTURE_2D, tex_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

    }





    void use() { glBindTexture(GL_TEXTURE_2D,tex_id); }

    operator GLuint() { return tex_id; }

};

}

#endif // TEXTURE_HPP
