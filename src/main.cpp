#include <iostream>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include <glm/glm.hpp>

#include "TextRenderer.h"
#include "Shader.h"

#include "App.h"

static const int WIDTH = 1920;
static const int HEIGHT = 500;

using namespace Renderer;

int main() {
	App app;
	app.init(WIDTH, HEIGHT);

	GLFWwindow* window = app.getWindow();

	TextRenderer* textRenderer = new TextRenderer(window, WIDTH, HEIGHT);
	textRenderer->init();
	textRenderer->loadFont("../assets/fonts/BigBlue_TerminalPlus.ttf", 24);

	auto texture = textRenderer->getFontTexture();

	float time = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		textRenderer->beginFrame();

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		time += deltaTime;

		glm::vec2 position = glm::vec2(20.0f, 20.0f);
		auto color = glm::vec3(0.5f, 0.5f, 0.9f);
		textRenderer->drawText(m_text, position, color);

		textRenderer->endFrame();
	}

	glfwTerminate();
	delete textRenderer;
	std::cout << "App closed succesfully" << std::endl;
	return 0;
}
