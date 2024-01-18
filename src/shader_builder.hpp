 #pragma once

#include "pch.hpp"
#include "shader.hpp"
#include "directory.hpp"

// utility ?


    // std::cout <<  "\nSHADERS HEADER:" << std::endl;
    // for (auto ubo:UBO::pool) { 

    //     std::string str = "layout (binding = ";
    //     str += std::to_string(ubo->binding);
    //     str += ", std140) uniform ";
    //     str += ubo->name;
    //     str += " { ";
        
    //     int i = 0; for (auto obj:ubo->buffer.objects) { 

    //         str += obj.name;
    //         str += " ";
    //         for(int i = 0; i < obj.name.length(); i++) str += std::tolower(obj.name[i]); 
    //         str += "[";
    //         str += std::to_string(obj.reserved);
    //         str += "]; ";

    //      }

    //     str += " };";
    //     std::cout <<  str << std::endl;

    // }


struct ShaderBuilder {

  Directory shaders; 

  ShaderBuilder() : shaders("/assets/shaders/") {

    for (auto &s:shaders.list) {

      // PLOGD << s.path;
    }
  }

};
