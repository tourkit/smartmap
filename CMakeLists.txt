cmake_minimum_required(VERSION 3.12)
project(TestZone VERSION 0.1.0)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(TestZone 

    main.cpp
    
    include/vendor/gl3w/src/gl3w.c

    # src/attribute.cpp
    src/window.cpp
    src/shader.cpp
    src/texture.cpp
    src/framebuffer.cpp
    src/file.cpp

    # include/vendor/RectangleBinPack/GuillotineBinPack.cpp
    # include/vendor/RectangleBinPack/Rect.cpp

)

# target_compile_features(TestZone PUBLIC cxx_std_20)
set_target_properties(TestZone PROPERTIES COMPILE_FLAGS "-std=c++17 ")


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)

configure_file(assets/shader/vertex_test.glsl assets/shader/ COPYONLY)
configure_file(assets/shader/fragment_test.glsl assets/shader/ COPYONLY)

target_include_directories(TestZone PRIVATE
    .
    src
    include
    include/vendor
    include/vendor/gl3w/include
    
    )

target_link_libraries(TestZone PRIVATE
    OpenGL::GL
    glfw
    ws2_32
)
