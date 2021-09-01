#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>

#include <set>
#include <map>
#include <vector>

#include <string>
#include <cstring>

#include <cmath>

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <thread>

#include <functional>

#include <iterator>
#include <regex>

#include <chrono>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <RectangleBinPack/GuillotineBinPack.h> 

#include "file.hpp"

struct vec2 { float x,y; };
struct vec4 { float x,y,z,w; };
struct RectF { vec2 size = {1,1}; vec2 pos = {0,0}; };


// void glInfo(GLenum glenum, std::string name = "Info") {

//     int data;

//     glGetIntegerv(glenum, &data);

//     std::cout << name << ": " << data <<std::endl;

// }

#endif