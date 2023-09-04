#include "TextRenderer.h"
#include "App.h"
#include "stdio.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include <glm/glm.hpp>
#include <string>
#include <functional>

using namespace Renderer;

std::string m_text;
int carretIndex = 0;

void characterCallback(GLFWwindow* window, unsigned int keyCode);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

App::App() {
}

App::~App() {
	glfwTerminate();
	std::cout << "App closed succesfully" << std::endl;
}

void App::init(int width, int height) {
	m_width = width;
	m_height = height;
    if (!glfwInit()) {
        printf("glfw init failed! \n");
        return;
    }

    m_window = glfwCreateWindow(m_width, m_height, "Nexus Engine", NULL, NULL);
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

    glfwSetCharCallback(m_window, characterCallback);
    glfwSetKeyCallback(m_window, keyCallback);
}

void App::run() {
	TextRenderer* textRenderer = new TextRenderer(m_window, m_width, m_height);
	textRenderer->init();
	textRenderer->loadFont("../assets/fonts/BigBlue_TerminalPlus.ttf", 24);

	auto texture = textRenderer->getFontTexture();

	float time = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(m_window)) {
		textRenderer->beginFrame();

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		time += deltaTime;

		glm::vec2 position = glm::vec2(20.0f, 20.0f);
		auto color = glm::vec3(0.5f, 0.5f, 0.9f);
		textRenderer->drawScreenQuad({0.0f, 0.0f}, m_width, m_height, color);
		textRenderer->drawText(m_text, position, color);
		auto texture = textRenderer->getFontTexture();
		textRenderer->setCarretIndex(carretIndex);

		textRenderer->endFrame();
	}
	delete textRenderer;
}

void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	if (carretIndex != m_text.size()) {
		m_text.insert(m_text.begin() + carretIndex, keyCode);
		carretIndex += 1;
	}
	else {
		m_text += keyCode;
		carretIndex += 1;
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_BACKSPACE && action != GLFW_RELEASE) {
		if (!m_text.empty() && carretIndex != 0) {
			m_text.erase(m_text.begin() + (carretIndex - 1));
			carretIndex -= 1;
		}
	}

	if (key == GLFW_KEY_DELETE && action != GLFW_RELEASE) {
		if (!m_text.empty() && carretIndex != 0) {
			m_text.erase(m_text.begin() + carretIndex);
		}
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		if (carretIndex != m_text.size()) {
			m_text.insert(m_text.begin() + carretIndex, '\n');
			carretIndex += 1;
		}
		else {
			m_text += '\n';
			carretIndex += 1;
		}
	}

    if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
		if (carretIndex > 0) {
			carretIndex -= 1;
		}
    }

	if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
		if (carretIndex < m_text.size()) {
			carretIndex += 1;
		}
	}
}
