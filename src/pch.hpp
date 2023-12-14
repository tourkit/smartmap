#ifndef GLOBALS_H
#define GLOBALS_H

#define DEBUG_GL  /// <= DISABLE BEFORE RELEASE

#include <iostream>

#include <set>
#include <map>
#include <vector>

#include <string>
#include <cstring>

#include <cmath>

#include <thread>

#include <functional>

#include <iterator>
#include <regex>

#include <chrono>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "include/vendor/glm/glm.hpp"

#include "include/vendor/plog/Log.h"
#include "include/vendor/plog/Initializers/RollingFileInitializer.h"

#include "file.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

struct FT { 

    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;

    GLuint width;
    GLuint height;
    
    void* buffer;

    FT(const char* chr, int size = 200) {
        
        FT_Init_FreeType(&library);
        
        FT_New_Face(library, "assets/Anonymous.ttf", 0, &face); 

        FT_Set_Pixel_Sizes(face, 0, size);
        
        slot = face->glyph;
        FT_Load_Char(face, *chr, FT_LOAD_RENDER);

        buffer = slot->bitmap.buffer;
        width = slot->bitmap.width;
        height = slot->bitmap.rows;

    }

    ~FT() {
        
        FT_Done_Face(face);
        FT_Done_FreeType(library);

    }

};



struct vec2 { float x = 0, y = 0; };
struct vec4 { float x = 0, y = 0, z = 0, w = 0; };


// -1 to 1
// std::array<float, 6> is width height pos_x pos_y 
static std::vector<std::array<float, 8>> matrice(unsigned int x, unsigned int y) {

    std::vector<std::array<float, 8>> mat;

    float xsize = 1. / x;
    float ysize = 1. / y;

    for (int row = 0; row < x; ++row) // for in for not in wrong order for opti ?
      for (int col = 0; col < y; ++col) 
        mat.push_back({xsize, ysize, xsize * row*2-(1-xsize),  ysize * col*2-(1-ysize), xsize * row,  ysize * col,0,0});

    return mat;

}


static void glInfo(GLenum glenum, std::string name = "Info") {

    int data;

    glGetIntegerv(glenum, &data);

    std::cout << name << ": " << data <<std::endl;

}

#ifdef DEBUG_GL
static std::map<GLenum,std::string> GL_errors = {

  { GL_NO_ERROR, "no error" },
  { GL_INVALID_ENUM, "invalid enumerant" },
  { GL_INVALID_VALUE, "invalid value" },
  { GL_INVALID_OPERATION, "invalid operation" },
  { GL_STACK_OVERFLOW, "stack overflow" },
  { GL_STACK_UNDERFLOW, "stack underflow" },
  { GL_OUT_OF_MEMORY, "out of memory" },
  { GL_NO_ERROR, "unknown" }

};
static void glError(std::string file_name, std::string function_name, GLuint line) {

  GLenum code = glGetError();

  if (code == GL_NO_ERROR) return;

  std::string code_string = "unknown";

  if (GL_errors.find(code) != GL_errors.end()) code_string  = GL_errors[code];

  file_name = file_name.substr(file_name.find_last_of("\\")+1);
  function_name = function_name.substr(function_name.find(" "));
  function_name = function_name.substr(0,function_name.find("("));

  std::cout << file_name << "(" << line << "): " << function_name  <<  "() => " << code_string << '\n';

  exit(0);

}

#define GL_ERROR() glError(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#define GL_PRINT(str) std::cout << str << std::endl;
#else
#define GL_ERROR()
#define GL_PRINT(str) /**/
#endif

#endif
