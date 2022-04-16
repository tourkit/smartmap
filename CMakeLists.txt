cmake_minimum_required(VERSION 3.12)
project(SmartMap VERSION 0.1.0)


add_executable(SmartMap 

    main.cpp

    include/vendor/flecs/flecs.c

)

target_compile_features(SmartMap PUBLIC cxx_std_20)
set_target_properties(SmartMap PROPERTIES COMPILE_FLAGS "-DIMGUI_IMPL_OPENGL_LOADER_GL3W  ")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)

target_include_directories(SmartMap PRIVATE
    .
    src
    include
    include/vendor
    include/vendor/gl3w/include
    
    )

target_link_libraries(SmartMap PRIVATE

ws2_32

)
