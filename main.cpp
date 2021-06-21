#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>

#include "TextRenderer.hpp"
#include "Shader.hpp"
#include <time.h>
#include <chrono>

int init() {
    if (!glfwInit()) {
        glfwTerminate();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 0;
}

int glfwWindowInit(GLFWwindow* window) {
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    if(glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    return 0;
}

static std::string text;

static std::string appendToString(char ch, std::string& text) {
    text+= ch;
    return text;
}

void characterCallback (GLFWwindow * window, unsigned int keyCode);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

const int WIDTH = 800;
const int HEIGHT = 500;
const double PI = 3.14159265359;

int main () {
    init();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Test Game", NULL, NULL);
    glfwWindowInit(window);
    glfwSetCharCallback(window, characterCallback);
    glfwSetKeyCallback(window, keyCallback);
    	
    //TextRenderer textRenderer(WIDTH, HEIGHT);
    TextRenderer *textRenderer = new TextRenderer(WIDTH, HEIGHT);
    //std::string fontPath = "/Users/pashaukolov/Library/Fonts/Roboto-regular.ttf";
    std::string fontPath = "/Users/pashaukolov/Library/Fonts/PixelFont.ttf";
    textRenderer->loadFont(fontPath, 24);
    glClearColor(0.0f, 0.05f, 0.08f, 0.0f);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    float xpos = 0;
    bool keyJustPressed = false;
    
    while(!glfwWindowShouldClose(window)) {
	//glfwGetCursorPos(window, &xpos, &ypos);
	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	// int state = glfwGetKey(window, GLFW_KEY_BACKSPACE);
	// if (state == GLFW_PRESS) {
	//     keyJustPressed = true;
	//     std::cout << "backspace" << std::endl;
	    
	// }

	static bool reload_key_pressed = false;
	bool down = glfwGetKey( window, GLFW_KEY_R );
	if ( down && !reload_key_pressed ) {
	    reload_key_pressed = true;
	} else if ( !down && reload_key_pressed ) {
	    reload_key_pressed = false;
	    textRenderer->reloadShader();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec2 position = glm::vec2(10.0f, 10.0f);

	textRenderer->drawText(text, deltaTime, position, glm::vec3(0.3f, 0.95f, 0.6f));

	// textRenderer->drawText("hello", deltaTime, position, glm::vec3(0.2f, 0.95f, 0.99f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    delete textRenderer;
    
    std::cout << "App closed succesfully" << std::endl;

    return 0;
}

void characterCallback (GLFWwindow * window, unsigned int keyCode) {
    text += keyCode;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS || action == GLFW_REPEAT) {
	text = text.substr(0, text.size()-1);
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
	text += '\n';
    }
}
