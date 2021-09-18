cmake_minimum_required(VERSION 3.12)
project(SmartMap VERSION 0.1.0)


add_executable(SmartMap 

    main.cpp
    
    
    include/vendor/ofxLibArtnet/artnet/artnet.c
    include/vendor/ofxLibArtnet/artnet/misc.c
    include/vendor/ofxLibArtnet/artnet/network.c
    include/vendor/ofxLibArtnet/artnet/receive.c
    include/vendor/ofxLibArtnet/artnet/tod.c
    include/vendor/ofxLibArtnet/artnet/transmit.c
    include/vendor/ofxLibArtnet/ofxLibArtnet.cpp



)

target_compile_features(SmartMap PUBLIC cxx_std_20)
set_target_properties(SmartMap PROPERTIES COMPILE_FLAGS " ")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)



target_include_directories(SmartMap PRIVATE
    .
    src
    include
    include/vendor
    
    )

target_link_libraries(SmartMap PRIVATE
  
    ws2_32
    netapi32
    dpnet
    iphlpapi
)
