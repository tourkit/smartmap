cmake_minimum_required(VERSION 3.12)
project(smartmap VERSION 0.1.0)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

add_executable(smartmap

    main.cpp

    src/texture.cpp
    src/window.cpp
    src/shader.cpp
    src/framebuffer.cpp
    src/vbo.cpp
    src/ubo.cpp
    src/atlas.cpp

    src/buffer.cpp
    src/engine.cpp
    src/stack.cpp

    src/dmx.cpp
    src/artnet.cpp

    src/file.cpp

    src/gui.cpp

    src/smartmap.cpp

    src/widgets/artnet_widget.cpp

    include/vendor/imgui/imgui.cpp
    include/vendor/imgui/imgui_tables.cpp
    include/vendor/imgui/imgui_widgets.cpp
    include/vendor/imgui/imgui_demo.cpp
    include/vendor/imgui/imgui_draw.cpp
    include/vendor/imgui/backends/imgui_impl_glfw.cpp
    include/vendor/imgui/backends/imgui_impl_opengl3.cpp

    include/vendor/gl3w/src/gl3w.c

    include/vendor/RectangleBinPack/MaxRectsBinPack.cpp
    include/vendor/RectangleBinPack/Rect.cpp

    include/vendor/ofxLibArtnet/artnet/artnet.c
    include/vendor/ofxLibArtnet/artnet/misc.c
    include/vendor/ofxLibArtnet/artnet/network.c
    include/vendor/ofxLibArtnet/artnet/receive.c
    include/vendor/ofxLibArtnet/artnet/tod.c
    include/vendor/ofxLibArtnet/artnet/transmit.c
    include/vendor/ofxLibArtnet/ofxLibArtnet.cpp
)

target_compile_features(smartmap PUBLIC cxx_std_20)
set_target_properties(smartmap PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(smartmap PROPERTIES COMPILE_FLAGS "-DIMGUI_IMPL_OPENGL_LOADER_GL3W -Xlinker --verbose ")

target_link_directories(smartmap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor/NDI/Lib/ )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor/NDI/Lib/Processing.NDI.Lib.x64.dll DESTINATION .)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)
file(COPY config.json DESTINATION .)

target_include_directories(smartmap PRIVATE
    .
    src
    include
    include/vendor/
    include/vendor/imgui/
    include/vendor/gl3w/include/
    include/vendors/glm/
    include/vendor/NDI/Include/
    ${FREETYPE_INCLUDE_DIRS}
    )

    link_directories(${FREETYPE_LIBRARY_DIRS})

target_link_libraries(smartmap PRIVATE
    OpenGL::GL
    glfw
    assimp
    Processing.NDI.Lib.x64
    ${FREETYPE_LIBRARIES}
)

if (WIN32)
target_link_libraries(smartmap PRIVATE
    ws2_32
    iphlpapi
    )
add_compile_definitions(WIN32="1")
endif (WIN32)

if(UNIX)
target_link_libraries(smartmap PRIVATE
    -ldl
    )
# for libartnet to use a less deprecated network api on linux
set_target_properties(smartmap PROPERTIES COMPILE_FLAGS "-DHAVE_GETIFADDRS -DHAVE_LINUX_IF_PACKET_H")
endif(UNIX)

#    netapi32
#    dpnet
#    Processing.NDI.Lib.x64
