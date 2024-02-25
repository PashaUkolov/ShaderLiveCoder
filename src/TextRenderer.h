#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <freetype/ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

struct Character {
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
    int texCoord;
};

struct Texture {
    unsigned int id;
    float width = 0;
	float height = 0;
};

struct TextureAtlasPart {
	float x, y, width, height;
};

void characterCallback(GLFWwindow* window, unsigned int keyCode);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

namespace Renderer {
	class TextRenderer {
	public:
		TextRenderer();
		~TextRenderer();

		void loadFont(const std::string& path, unsigned int size);
		Texture getFontTexture() { return m_fontAtlas; };

		void init(GLFWwindow* window, int screenWidth, int screenHeight);

		void beginFrame();
		void endFrame();

		void draw();
		void drawQuad(glm::vec2 position, float width, float height, glm::vec3 color);
		void drawScreenQuad(glm::vec2 position, float width, float height, float time);
		void drawQuadTexture(Texture tex, glm::vec2 position, float width, float height, TextureAtlasPart part, glm::vec3 color);
		void drawText(const std::string text, glm::vec2 position, glm::vec3 color);

		void setText(const std::string& text);

		int getVisibleLinesCount();

		void calculateTextNewLineIndices();

		glm::vec2 getTextPositionFromIndex(int index);
		void setCarretIndex(int index);
		void setLineNumber(int lineNumber);
		void drawCarret(glm::vec2 position, glm::vec3 color);
		void reloadShader();
		std::unordered_map<char, Character> characters;

		void characterInput(GLFWwindow* window, unsigned int keyCode);
		void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		float m_width, m_height;
		GLFWwindow* m_window = nullptr;
		Shader m_glyphShader;
		Shader m_quadShader;
		Shader m_screenQuadShader;
		unsigned int m_VBO, m_VAO, m_EBO;
		Texture m_fontAtlas;
		glm::vec2 m_carretPosition = {};
		glm::vec2 m_textPosition = {};
		float scrollAmmount = 0.0f;
		int m_carretIndex = 0;
		int m_lineNumber = 0;
		int m_selectionStartIndex = 0;
		int m_selectionEndIndex = 0;
		bool m_isSelecting = false;
		std::string m_text;
		std::vector<int> m_newLineIndices;
	};
}
    
