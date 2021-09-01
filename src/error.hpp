#ifndef ERROR_H
#define ERROR_H

#include <GL/gl3w.h>

#include <map>

#define DEBUG_GL

#ifdef DEBUG_GL
std::map<GLenum,std::string> GL_errors = {

  { GL_NO_ERROR, "no error" },
  { GL_INVALID_ENUM, "invalid enumerant" },
  { GL_INVALID_VALUE, "invalid value" },
  { GL_INVALID_OPERATION, "invalid operation" },
  { GL_STACK_OVERFLOW, "stack overflow" },
  { GL_STACK_UNDERFLOW, "stack underflow" },
  { GL_OUT_OF_MEMORY, "out of memory" },
  { GL_NO_ERROR, "unknown" }

};
void glError(std::string file_name, std::string function_name, GLuint line) {

  GLenum code = glGetError();

  if (code == GL_NO_ERROR) return;

  std::string code_string = "unknown";

  if (GL_errors.find(code) != GL_errors.end()) code_string  = GL_errors[code];

  file_name = file_name.substr(file_name.find_last_of("\\")+1);
  function_name = function_name.substr(function_name.find(" "));
  function_name = function_name.substr(0,function_name.find("("));

  std::cout << file_name << "(" << line << "): " << function_name  <<  "() => " << code_string << '\n';

}

#define GL_ERROR() glError(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#else
#define GL_ERROR()
#endif

#endif