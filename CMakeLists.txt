cmake_minimum_required(VERSION 3.12)
project(SmartMap VERSION 0.1.0)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(SmartMap 

    main.cpp
    
    src/texture.cpp
    src/window.cpp
    src/shader.cpp
    src/framebuffer.cpp
    src/vbo.cpp
    src/ubo.cpp
    src/atlas.cpp

    src/device/artnet.cpp;
    
    src/file.cpp
    
    src/gui.cpp

    include/vendor/imgui/imgui.cpp
    include/vendor/imgui/imgui_widgets.cpp
    include/vendor/imgui/imgui_draw.cpp
    include/vendor/imgui/imgui_impl_glfw.cpp
    include/vendor/imgui/imgui_impl_opengl3.cpp    
    
    include/vendor/gl3w/src/gl3w.c

    include/vendor/RectangleBinPack/GuillotineBinPack.cpp
    include/vendor/RectangleBinPack/Rect.cpp

    include/vendor/ofxLibArtnet/artnet/artnet.c
    include/vendor/ofxLibArtnet/artnet/misc.c
    include/vendor/ofxLibArtnet/artnet/network.c
    include/vendor/ofxLibArtnet/artnet/receive.c
    include/vendor/ofxLibArtnet/artnet/tod.c
    include/vendor/ofxLibArtnet/artnet/transmit.c
    include/vendor/ofxLibArtnet/ofxLibArtnet.cpp

    

)

target_compile_features(SmartMap PUBLIC cxx_std_20)
set_target_properties(SmartMap PROPERTIES COMPILE_FLAGS "-DIMGUI_IMPL_OPENGL_LOADER_GL3W  ")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)

configure_file(assets/shader/blur_x.comp assets/shader/ COPYONLY)
configure_file(assets/shader/blur_y.comp assets/shader/ COPYONLY)
configure_file(assets/shader/smartmap.vert assets/shader/ COPYONLY)
configure_file(assets/shader/smartmap.frag assets/shader/ COPYONLY)

target_include_directories(SmartMap PRIVATE
    .
    src
    include
    include/vendor
    include/vendor/gl3w/include
    
    )

target_link_libraries(SmartMap PRIVATE
    OpenGL::GL
    glfw
    ws2_32
    netapi32
    dpnet
    iphlpapi
)
