cmake_minimum_required(VERSION 3.12)
project(TestZone VERSION 0.1.0)


# include(CTest)
# enable_testing()

add_executable(TestZone 

    main.cpp

    src/attribute.cpp

    src/test.cpp

    include/vendor/RectangleBinPack/GuillotineBinPack.cpp
    include/vendor/RectangleBinPack/Rect.cpp

)

set_target_properties(TestZone PROPERTIES COMPILE_FLAGS "-std=c++17")


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(TestZone PRIVATE
    .
    src
    include
    include/vendor
    
    )