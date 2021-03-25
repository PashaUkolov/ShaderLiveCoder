#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader {
public:
    Shader parseShader(std::string filePath);
    void compileShader(const char* vertexSource, const char* fragmentSource);
    Shader reloadShader(std::string filePath);
    void checkErrors(unsigned int object, std::string type);

    GLuint ID;
};
