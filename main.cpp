#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "TextRenderer.hpp"

class Shader {
public:
    Shader() {
	
    }
    
private:
    unsigned int shaderID;
};

class ShaderParser {
public:
    ShaderParser() {
    }

    Shader loadShaderfromFile(const std::string& filepath) {
	
    }
    
private:
};

int main () {    
    if (!glfwInit()) {
        glfwTerminate();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 500, "Test Game", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    if(glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    TextRenderer textRenderer;
    textRenderer.loadFont("/Users/pashaukolov/Library/Fonts/PixelFont.ttf", 24);
    
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    std::string name = "App closed succesfully";
    std::cout << name << std::endl;

    return 0;
}
