#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>


#include "window.hpp"
#include "shader.hpp"

GL::Window* window = new GL::Window(false,900,600);

int main() {

GL::ShaderProgram shader("assets/shader/vertex_test.glsl", "assets/shader/fragment_test.glsl");

while(true) window->render([&]() {
    

    shader.use();


});

} 


