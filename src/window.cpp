#include "window.hpp"
#include "engine.hpp"
#include "framebuffer.hpp"
#include "log.hpp"
#include "gui.hpp"

#include <chrono>
#include <thread>


Window::Window(uint16_t width, uint16_t height, uint16_t offset_x, uint16_t offset_y, bool fullscreen)

    : fullscreen(fullscreen) , width(width) , height(height) , offset_x(offset_x) , offset_y(offset_y) , fps("Window") {

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

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    const GLFWvidmode* mode = glfwGetVideoMode(monitors[0]);
    uint16_t refreshRate = glfwGetVideoMode(monitors[0])->refreshRate;

    displays.push_back(Display(mode->width, mode->height,refreshRate));
    PLOGI  << " Display @ " << refreshRate << "Hz " << mode->width << "x" << mode->height;

    if (fullscreen)
    {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//        window = glfwCreateWindow(mode->width, mode->height, "OUTPUT", monitor, nullptr);
        id = glfwCreateWindow(1920, 1200, "OUTPUT", monitor, nullptr);
        cursor_visibility = false;
    }
    else
    {

        id = glfwCreateWindow(width, height, "OUTPUT", nullptr, nullptr);

    }


    glfwSetWindowUserPointer(id, this);
#ifdef __linux__
#ifdef NDEBUG
    glfwSetInputMode(id, GLFW_CURSOR, (cursor_visibility ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
#endif
#endif
    if (uid_callbacks) {
        keypress();
    }

    glfwMakeContextCurrent(id);

    glfwSwapInterval(1); // VSYNC

    gl3wInit();

    if (!fullscreen)setPos(offset_x, offset_y);

    glEnable(GL_BLEND);
}

static void framebuffer_size_callback(GLFWwindow* id, int width, int height) { glViewport(0, 0, width, height); }

void Window::updatePos() {

    glfwSetWindowPos(id, offset_x, offset_y);
    glfwSetFramebufferSizeCallback(id, framebuffer_size_callback);
    glfwSetWindowSize(id, width, height);

}
void Window::setPos(uint16_t offset_x, uint16_t offset_y) {
    this->offset_x = offset_x;
    this->offset_y = offset_y;
    updatePos();
}

void Window::updateSize() {
    glfwSetWindowSize(id, width, height);
    glViewport(0, 0, width, height);
}
void Window::setSize(uint16_t width, uint16_t height) {
    this->width = width;
    this->height = height;
    updateSize();
}


Window::~Window() { glfwTerminate(); }

void Window::keypress() {
    glfwSetKeyCallback(id, [](GLFWwindow* id, int key, int scancode, int action, int mode) {
        (void)scancode;
        (void)mode;
        auto _this = (Window*)glfwGetWindowUserPointer(id);
        if (action == GLFW_PRESS) {

            if (_this->keypress_cbs.find(key) != _this->keypress_cbs.end()) return _this->keypress_cbs[key](key);

            PLOGD << "key " << (int)key ;

        }
    });
    glfwSetCursorPosCallback(id, [](GLFWwindow* id, double mouse_x, double mouse_y) {
        auto _this = (Window*)glfwGetWindowUserPointer(id);
        _this->mouse_x = mouse_x;
        _this->mouse_y = mouse_y;
    });
    glfwSetMouseButtonCallback(id, [](GLFWwindow* id, int button, int action, int mods) {
        (void)button;
        (void)mods;
        auto _this = (Window*)glfwGetWindowUserPointer(id);
        if (action == GLFW_PRESS) {
            // mouse click
             _this->clickCallBack();
        }
    });
}


void Window::render(std::function<void()> callback) {

    glfwPollEvents();

    // fps.run(max_fps);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // BG COLOR
    glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

    callback();

    glfwSwapBuffers(id);
}
