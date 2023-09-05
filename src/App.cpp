#include "TextRenderer.h"
#include "App.h"
#include "stdio.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include <glm/glm.hpp>
#include <string>
#include <functional>

using namespace Renderer;

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
}

void App::run() {
	TextRenderer* textRenderer = new TextRenderer(m_window, m_width, m_height);
	textRenderer->init();
	textRenderer->loadFont("../assets/fonts/BigBlue_TerminalPlus.ttf", 24);

	auto texture = textRenderer->getFontTexture();

	float time = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	Shader mainShader;
	m_text = mainShader.getShaderText("../assets/shaders/screenQuad.glsl");

	while (!glfwWindowShouldClose(m_window)) {
		textRenderer->beginFrame();

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		time += deltaTime;

		glm::vec2 position = glm::vec2(20.0f, 20.0f);
		auto color = glm::vec3(0.5f, 0.5f, 0.9f);
		//textRenderer->drawScreenQuad({0.0f, 0.0f}, m_width, m_height, color);
		textRenderer->drawText(m_text, position, color);
		/*textRenderer->setCarretIndex(carretIndex);
		textRenderer->setLineNumber(carretIndex);*/

		textRenderer->endFrame();
	}
	delete textRenderer;
}