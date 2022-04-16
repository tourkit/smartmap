cmake_minimum_required(VERSION 3.12)
project(SmartMap VERSION 0.1.0)


add_executable(SmartMap 

    main.cpp

)

target_compile_features(SmartMap PUBLIC cxx_std_20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)

target_include_directories(SmartMap PRIVATE
    .
    src
    include
    include/vendor
    
    )

target_link_libraries(SmartMap PRIVATE

ws2_32

)
