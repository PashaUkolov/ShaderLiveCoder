#include "InputHandler.h"
#include <string>
#include <vector>

std::string m_text;
int carretIndex = 0;
int lineNumber = 0;

void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	if (carretIndex != m_text.size()) {
		m_text.insert(m_text.begin() + carretIndex, keyCode);
		carretIndex += 1;
	}
	else {
		m_text += keyCode;
		carretIndex += 1;
	}

	//maybe there is better way to tracks new line indeces
	/*newLineIndices.clear();
	for (int i = 0; i < m_text.size(); i++) {
		if (m_text[i] == '\n') {
			newLineIndices.push_back(i);
		}
	}*/
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_BACKSPACE && action != GLFW_RELEASE) {
		if (!m_text.empty() && carretIndex != 0) {
			m_text.erase(m_text.begin() + (carretIndex - 1));
			carretIndex -= 1;
		}

		//maybe there is better way to tracks new line indeces
		/*newLineIndices.clear();
		for (int i = 0; i < m_text.size(); i++) {
			if (m_text[i] == '\n') {
				newLineIndices.push_back(i);
			}
		}*/
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

	if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
		lineNumber -= 1;
		if (lineNumber < 0) {
			lineNumber = 0;
		}
		//carretIndex = newLineIndices[lineNumber];
	}

	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
		lineNumber += 1;
		//carretIndex = newLineIndices[lineNumber];
	}
}

InputHandler::InputHandler(GLFWwindow* window) {
	m_window = window;
}

void InputHandler::init() {
	glfwSetCharCallback(m_window, characterCallback);
	glfwSetKeyCallback(m_window, keyCallback);
}
