cmake_minimum_required(VERSION 3.12)

project(smartmap VERSION 0.1.0)

option(PLUGREGIE "c moi" OFF)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
# find_package(Freetype REQUIRED)

add_executable( smartmap

src/log.cpp

main.cpp

# src/test.cpp

src/folder.cpp
src/file.cpp
src/json.cpp
src/image.cpp

src/thread.cpp
src/messagequeue.cpp

vendors/gl3w/src/gl3w.c

src/node.cpp
src/member.cpp
src/instance.cpp

src/vbo.cpp
src/ubo.cpp
src/output.cpp

src/texture.cpp
src/framebuffer.cpp
src/builder.cpp
src/shader.cpp
src/window.cpp

src/open.cpp
src/save.cpp
src/engine.cpp

src/ndi.cpp


src/callbacks.cpp
src/editor.cpp
src/editors.cpp


src/effector.cpp
src/model.cpp

src/remap.cpp

src/layer.cpp
# src/fps.cpp
src/drawcall.cpp
src/atlas.cpp

src/tree.cpp

src/gui.cpp
vendors/imgui/imgui.cpp
vendors/imgui/imgui_tables.cpp
vendors/imgui/imgui_widgets.cpp
vendors/imgui/imgui_demo.cpp
vendors/imgui/imgui_draw.cpp
vendors/imgui/backends/imgui_impl_glfw.cpp
vendors/imgui/backends/imgui_impl_opengl3.cpp
vendors/ImGuiColorTextEdit/TextEditor.cpp


src/artnet.cpp
vendors/ofxLibArtnet/artnet/artnet.c
vendors/ofxLibArtnet/artnet/misc.c
vendors/ofxLibArtnet/artnet/network.c
vendors/ofxLibArtnet/artnet/receive.c
vendors/ofxLibArtnet/artnet/tod.c
vendors/ofxLibArtnet/artnet/transmit.c
vendors/ofxLibArtnet/ofxLibArtnet.cpp

vendors/tinyexpr/tinyexpr.cpp

vendors/RectangleBinPack/Rect.cpp
vendors/RectangleBinPack/MaxRectsBinPack.cpp


vendors/ft/builds/windows/ftdebug.c
vendors/ft/builds/windows/ftsystem.c
vendors/ft/src/base/ftver.rc
vendors/ft/src/autofit/autofit.c
vendors/ft/src/base/ftbase.c
vendors/ft/src/base/ftbbox.c
vendors/ft/src/base/ftbdf.c
vendors/ft/src/base/ftbitmap.c
vendors/ft/src/base/ftcid.c
vendors/ft/src/base/ftfstype.c
vendors/ft/src/base/ftgasp.c
vendors/ft/src/base/ftglyph.c
vendors/ft/src/base/ftgxval.c
vendors/ft/src/base/ftinit.c
vendors/ft/src/base/ftmm.c
vendors/ft/src/base/ftotval.c
vendors/ft/src/base/ftpatent.c
vendors/ft/src/base/ftpfr.c
vendors/ft/src/base/ftstroke.c
vendors/ft/src/base/ftsynth.c
vendors/ft/src/base/fttype1.c
vendors/ft/src/base/ftwinfnt.c
vendors/ft/src/bdf/bdf.c
vendors/ft/src/bzip2/ftbzip2.c
vendors/ft/src/cache/ftcache.c
vendors/ft/src/cff/cff.c
vendors/ft/src/cid/type1cid.c
vendors/ft/src/gzip/ftgzip.c
vendors/ft/src/lzw/ftlzw.c
vendors/ft/src/pcf/pcf.c
vendors/ft/src/pfr/pfr.c
vendors/ft/src/psaux/psaux.c
vendors/ft/src/pshinter/pshinter.c
vendors/ft/src/psnames/psnames.c
vendors/ft/src/raster/raster.c
vendors/ft/src/sdf/sdf.c
vendors/ft/src/sfnt/sfnt.c
vendors/ft/src/smooth/smooth.c
vendors/ft/src/svg/svg.c
vendors/ft/src/truetype/truetype.c
vendors/ft/src/type1/type1.c
vendors/ft/src/type42/type42.c
vendors/ft/src/winfonts/winfnt.c
)
target_compile_features(smartmap PUBLIC cxx_std_20)
set_target_properties(smartmap PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(
  smartmap PROPERTIES COMPILE_FLAGS
                      "-DIMGUI_IMPL_OPENGL_LOADER_GL3W -Xlinker --verbose ")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY assets DESTINATION .)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
# include_directories(${})
include_directories(${CMAKE_SOURCE_DIR}/vendors)
add_definitions(-DFT2_BUILD_LIBRARY)
target_include_directories(
  smartmap
  PRIVATE .
          src
          vendors/imgui/
          vendors/gl3w/include/
          vendors/glm/
          vendors/ft/include/
          # ${FREETYPE_INCLUDE_DIRS}

)

# link_directories(${FREETYPE_LIBRARY_DIRS})

target_link_libraries(
  smartmap PRIVATE OpenGL::GL glfw assimp # ${FREETYPE_LIBRARIES}
)

target_link_directories(smartmap PRIVATE
                        ${CMAKE_CURRENT_SOURCE_DIR}/vendors/NDI/Lib/
                        vendors/
                        )

if(WIN32)
  file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/vendors/NDI/Lib/Processing.NDI.Lib.x64.dll
    DESTINATION .)
  target_include_directories(smartmap PRIVATE vendors/NDI/Include/)
  target_link_libraries(smartmap PRIVATE ws2_32 iphlpapi Processing.NDI.Lib.x64)
  add_compile_definitions(WIN32="1")
endif(WIN32)

if(UNIX)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/vendors/NDI/Lib/libndi.so DESTINATION .)
  file(CREATE_LINK libndi.so ${CMAKE_CURRENT_BINARY_DIR}/libndi.so.4
       COPY_ON_ERROR SYMBOLIC)
  target_link_libraries(smartmap PRIVATE -ldl -lndi)
  # for libartnet to use a less deprecated network api on linux
  set_target_properties(
    smartmap PROPERTIES COMPILE_FLAGS
                        "-DHAVE_GETIFADDRS -DHAVE_LINUX_IF_PACKET_H")
    target_compile_definitions(smartmap PRIVATE ISUNIX=1)
endif(UNIX)


target_compile_definitions(smartmap PRIVATE ROCH=1)
if(PLUGREGIE)
  target_compile_definitions(smartmap PRIVATE PLUGREGIE=1)
endif(PLUGREGIE)
