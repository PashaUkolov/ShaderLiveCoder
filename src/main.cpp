#include <iostream>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include <glm/glm.hpp>

#include "TextRenderer.h"
#include "Shader.h"

#include "App.h"

const int WIDTH = 800;
const int HEIGHT = 500;
const double PI = 3.14159265359;

int main() {
	App app;
	app.init();

	TextRenderer* textRenderer = new TextRenderer(WIDTH, HEIGHT);
	textRenderer->loadFont("../assets/fonts/pico-8.ttf", 24);
	glClearColor(0.0f, 0.05f, 0.08f, 0.0f);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	GLFWwindow* window = app.getWindow();
	while (!glfwWindowShouldClose(window)) {
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec2 position = glm::vec2(10.0f, 10.0f);
		textRenderer->drawText(m_text, deltaTime, position, glm::vec3(0.3f, 0.95f, 0.6f));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	delete textRenderer;

	std::cout << "App closed succesfully" << std::endl;

	return 0;
}
