#include "window.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <thread>

namespace GL {

Window::Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x, uint16_t offset_y)

    : fullscreen(fullscreen) , width(width) , height(height) , offset_x(offset_x) , offset_y(offset_y) {

    createView(0, 1, 0, 1);


}

Window::~Window() { glfwTerminate(); }

void Window::createView(int8_t windows_border, int8_t window_on_top, int8_t cursor_visibility, int8_t uid_callbacks) {


    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, (windows_border ? GLFW_TRUE : GLFW_FALSE));
    glfwWindowHint(GLFW_FLOATING, (window_on_top ? GLFW_TRUE : GLFW_FALSE));

    if (fullscreen)
    {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//        window = glfwCreateWindow(mode->width, mode->height, "OUTPUT", monitor, nullptr);
        window = glfwCreateWindow(1920, 1200, "OUTPUT", monitor, nullptr);
        cursor_visibility = false;
    }
    else
    {
        window = glfwCreateWindow(width, height, "OUTPUT", nullptr, nullptr);
        glfwSetWindowPos(window, offset_x, offset_y);
    }

    glfwSetWindowUserPointer(window, this);
#ifdef __linux__
#ifdef NDEBUG
    glfwSetInputMode(window, GLFW_CURSOR, (cursor_visibility ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
#endif
#endif
    if (uid_callbacks) {
        initUidCallbacks();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // VSYNC KILL VOUDOUJOUJOU
    if (gl3wInit()) {
        std::cout <<  "failed to initialize OpenGL" << std::endl;
        return;
    }
    if (!gl3wIsSupported(3, 2)) {
        std::cout <<  "OpenGL 3.2 not supported" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::initUidCallbacks() {
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
        (void)scancode;
        (void)mode;
        auto _this = (Window*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            std::cout << "keypress : " << (int)key << "\n";
            switch (key) {
            case GLFW_KEY_ESCAPE:
                _this->stopped = true;
                break;
            case GLFW_KEY_P:
                std::cout << "outprout\n";
                break;
            }
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double mouse_x, double mouse_y) {
        auto _this = (Window*)glfwGetWindowUserPointer(window);
        _this->mouse_x = mouse_x;
        _this->mouse_y = mouse_y;
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        (void)button;
        (void)mods;
        auto _this = (Window*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            // mouse click
             _this->clickCallBack();
        }
    });
}

void Window::render(std::function<void()> callback) {

    getFPS();

    iTime = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now()-start_c).count();

    // glfwMakeContextCurrent(window);  glewInit(); // MUTLI WINDOW ATTEMPT

    //glClearColor(0.0f, 0.0f, 0.0f, 0.1f); // BG COLOR

  //  glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

    callback();

    glfwPollEvents();

    glfwSwapBuffers(window);

}



void Window::readRGB(char *rgb, uint16_t width, uint16_t height, uint16_t x, uint16_t y, uint8_t mode) {

    glReadPixels(x,y,width,height,GL_RGB,GL_BYTE,rgb);

}

void Window::getFPS() {

    double time = glfwGetTime();
    double deltaTime = time - lastTime;

    while (deltaTime <= maxPeriod ) {

        std::this_thread::sleep_for(std::chrono::microseconds(10));

        time  = glfwGetTime();
        deltaTime = time - lastTime;

    }

    auto now = glfwGetTime();
    fps = (60.0/((now - lastTime)*60.0));
    lastTime = now;

}

}
