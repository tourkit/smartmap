cmake_minimum_required(VERSION 3.0.0)
project(TestMake VERSION 0.1.0)


include(CTest)
enable_testing()

add_executable(TestMake 

    main.cpp

    include/vendor/RectangleBinPack/GuillotineBinPack.cpp
    include/vendor/RectangleBinPack/Rect.cpp

)

set_target_properties(TestMake PROPERTIES COMPILE_FLAGS "-std=c++17")


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(TestMake PRIVATE
    .
    src
    include/vendor
    
    )
    
    
# target_link_libraries(TestMake -Linclude/vendor/)
