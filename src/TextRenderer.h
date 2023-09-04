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

namespace Renderer {
	class TextRenderer {
	public:
		TextRenderer(GLFWwindow* window, int screenWidth, int screenHeight);
		~TextRenderer();
		void loadFont(const std::string& path, unsigned int size);
		Texture getFontTexture() { return m_fontAtlas; };

		void init();

		void beginFrame();
		void endFrame();

		void writeVertexToBuffer(float vx, float vy, float vz);

		void drawQuad(glm::vec2 position, float width, float height, glm::vec3 color);
		void drawQuadTexture(Texture tex, glm::vec2 position, float width, float height, TextureAtlasPart part, glm::vec3 color);
		void drawText(std::string text, glm::vec2 position, glm::vec3 color);

		void drawCarret(glm::vec2 position, glm::vec3 color, float time);
		void reloadShader();
		std::unordered_map<char, Character> characters;
	private:
		float m_width, m_height;
		GLFWwindow* m_window = nullptr;
		Shader m_glyphShader;
		Shader m_carretShader;
		Shader m_quadShader;
		unsigned int m_VBO, m_VAO, m_EBO;
		Texture m_fontAtlas;
	};
}
    
