#pragma once

#include "glew/glew.h"
#include "glfw/glfw3.h"

void characterCallback(GLFWwindow* window, unsigned int keyCode);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class InputHandler {
public:
	InputHandler(GLFWwindow* window);
	void init();
private:
	GLFWwindow* m_window = nullptr;
};