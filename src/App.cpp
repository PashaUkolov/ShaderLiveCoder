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

    m_window = glfwCreateWindow(m_width, m_height, "Shader Livecoder", NULL, NULL);
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
	TextRenderer* textRenderer = new TextRenderer();
	textRenderer->init(m_window, m_width, m_height);
	textRenderer->loadFont("../assets/fonts/BigBlue_TerminalPlus.ttf", 20);

	glfwSetWindowUserPointer(m_window, textRenderer);

	float time = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	Shader mainShader;
	m_text = mainShader.getShaderText("../assets/shaders/screenQuad.glsl");
	textRenderer->setText(m_text);

	while (!glfwWindowShouldClose(m_window)) {
		textRenderer->beginFrame();

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		time += deltaTime;

		textRenderer->drawScreenQuad({ 0.0f, 0.0f }, m_width, m_height, time);
		textRenderer->draw();

		textRenderer->endFrame();
	}
	delete textRenderer;
}