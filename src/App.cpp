#include "App.h"

#include "stdio.h"

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include <string>
#include <functional>

std::string m_text;

void characterCallback(GLFWwindow* window, unsigned int keyCode);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

App::App() {
}

App::~App() {
}

void App::init(int width, int height) {
    if (!glfwInit()) {
        printf("glfw init failed! \n");
        return;
    }

    m_window = glfwCreateWindow(width, height, "Nexus Engine", NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        printf("window init failed! \n");
        return;
    }

    glfwMakeContextCurrent(m_window);

    if (glewInit() != GLEW_OK) {
        printf("glew init failed! \n");
        return;
    }

    //typedef void (*GLFWcharfun)(GLFWwindow*, unsigned int);

    glfwSetCharCallback(m_window, characterCallback);
    glfwSetKeyCallback(m_window, keyCallback);
}

void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	m_text += keyCode;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS || action == GLFW_REPEAT) {
		m_text = m_text.substr(0, m_text.size() - 1);
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		m_text += '\n';
	}
}
