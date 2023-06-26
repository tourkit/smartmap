#include "window.hpp"
#include "shader.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <thread>

namespace GL {

Window::Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x, uint16_t offset_y)

    : fullscreen(fullscreen) , width(width) , height(height) , offset_x(offset_x) , offset_y(offset_y) {

    int8_t windows_border = 0;
    int8_t window_on_top = 1; 
    int8_t cursor_visibility = 0;
    int8_t uid_callbacks = 1;


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
        setPos(offset_x, offset_y);
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
    
    lastTime = glfwGetTime();

}

void Window::updatePos() {  glfwSetWindowPos(window, offset_x, offset_y); }
void Window::setPos(uint16_t offset_x, uint16_t offset_y) { 
    this->offset_x = offset_x;
    this->offset_y = offset_y;
    updatePos();
}

void Window::updateSize() { glfwSetWindowSize(window, width, height); }
void Window::setSize(uint16_t width, uint16_t height) { 
    this->width = width;
    this->height = height;
    updateSize();
}


void SetTheFrameBufferSize(GLFWwindow* window, int h, int w)
{
	// Resize window
    // [](GLFWwindow* window, int width, int height){  
        std::cout << w << std::endl;
        //  }

}



Window::~Window() { glfwTerminate(); }

void Window::initUidCallbacks() {
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
        (void)scancode;
        (void)mode;
        auto _this = (Window*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            switch (key) {
            case GLFW_KEY_ESCAPE:
                exit(0);
                break;
            case GLFW_KEY_R:
                delete ShaderProgram::pool[0];
                ShaderProgram::pool[0] = new ShaderProgram({"C:/msys64/home/SysErr/old/smartmap/assets/shader/basic.vert", "C:/msys64/home/SysErr/old/smartmap/assets/shader/test.frag"});
                break;
            default:
                std::cout << "keypress : " << (int)key << "\n";
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

    if (glfwGetTime() - lastTime <= 1./280. ) return;
    
    this->lastTime = glfwGetTime();

    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // BG COLOR
    glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

    callback();

    glfwPollEvents();

    glfwSwapBuffers(window);

}

}
