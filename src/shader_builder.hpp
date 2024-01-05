 #pragma once

#include "pch.hpp"
#include "shader.hpp"

// utility ?

struct ShaderBuilder {

  Directory shaders; 

  ShaderBuilder() : shaders(REPO_DIR+"/assets/shaders/") {

    for (auto s:shaders.list) {

      PLOGD << s;
    }
  }

};
