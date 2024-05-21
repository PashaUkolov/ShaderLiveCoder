#include "TextRenderer.h"
using namespace Renderer;

TextRenderer::TextRenderer()  {
}

TextRenderer::~TextRenderer() {
}

void TextRenderer::loadFont(const std::string& path, unsigned int fontSize) {
	FT_Library lib;
	FT_Face face;

	FT_Error error = FT_Init_FreeType(&lib);

	if (error)
		printf("an error occuret duting fritype initializaton \n");

	error = FT_New_Face(lib, path.c_str(), 0, &face);

	if (error == FT_Err_Unknown_File_Format) {
		printf("font format is unsupported");
	}
	else if (error) {
		printf("font file couldn't be opened or read, or it is broken...");
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);;

	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			printf("ERROR::FREETYTPE: Failed to load Glyph \n");

		int height = face->glyph->bitmap.rows;

		Character character = {
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x),
			m_fontAtlas.width
		};

		characters.insert(std::pair<char, Character>(c, character));

		//get highest char
		if (height > m_fontAtlas.height)
			m_fontAtlas.height = height;

		m_fontAtlas.width += face->glyph->bitmap.width;
	}

	unsigned char* map = (unsigned char*)calloc(m_fontAtlas.width * m_fontAtlas.height, 1);
	int offset = 0;
	for (GLubyte c = 0; c < 128; c++) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		FT_Bitmap* bmp = &face->glyph->bitmap;

		int rows = bmp->rows;
		int width = bmp->width;

		for (int row = 0; row < rows; ++row) {
			for (int i = 0; i < width; ++i) {
				if (map) {
					map[row * int(m_fontAtlas.width) + offset + i] = bmp->buffer[row * bmp->pitch + i];
				}
			}
		}
		offset += width;
	}

	glGenTextures(1, &m_fontAtlas.id);
	glBindTexture(GL_TEXTURE_2D, m_fontAtlas.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_fontAtlas.width, m_fontAtlas.height, 0, GL_RED, GL_UNSIGNED_BYTE, map);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

void TextRenderer::init(GLFWwindow* window, int screenWidth, int screenHeight) {
	m_window = window;
	m_width = screenWidth;
	m_height = screenHeight;

	glfwSetCharCallback(m_window, characterCallback);
	glfwSetKeyCallback(m_window, keyCallback);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
	//glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	m_glyphShader = m_glyphShader.parseShader("../assets/shaders/glyph.glsl");
	m_quadShader = m_quadShader.parseShader("../assets/shaders/quad.glsl");
	m_screenQuadShader = m_screenQuadShader.parseShader("../assets/shaders/screenQuad.glsl");

	glUseProgram(m_glyphShader.ID);
	glUseProgram(m_quadShader.ID);
	glUseProgram(m_screenQuadShader.ID);

	glUniform2f(glGetUniformLocation(m_screenQuadShader.ID, "iResolution"), m_width, m_height);
}

void TextRenderer::beginFrame() {
	glClearColor(0.0f, 0.05f, 0.08f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TextRenderer::endFrame() {
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Renderer::TextRenderer::draw() {
	if (m_isSelecting) {
		auto selectionStartPosition = getTextPositionFromIndex(m_selectionStartIndex);
		auto selectionEndPosition = getTextPositionFromIndex(m_selectionEndIndex);
		drawQuad(selectionStartPosition, selectionEndPosition.x - selectionStartPosition.x, 15.0f, { 1.0f, 1.0f, 1.0f });
	}

	glm::vec2 position = glm::vec2(20.0f, 20.0f);
	auto color = glm::vec3(0.7f, 0.7f, 0.8f);
	drawText(m_text, position, color);
}

void TextRenderer::drawQuad(glm::vec2 position, float width, float height, glm::vec3 color) {
	float w = width;
	float h = height;
	float xpos = position.x;
	float ypos = position.y;
	float vertices[] = {
		xpos,  ypos + h,     0.0f, 0.0f,  // top right
		xpos, ypos,          0.0f, 0.0f,  // bottom right
		w + xpos, ypos,      0.0f, 0.0f,  // bottom left
		w + xpos,  ypos + h, 0.0f, 0.0f   // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glUseProgram(m_quadShader.ID);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
	glUniform3f(glGetUniformLocation(m_quadShader.ID, "quadColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(m_quadShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
}

void TextRenderer::drawScreenQuad(glm::vec2 position, float width, float height, float time) {
	float w = width;
	float h = height;
	float xpos = position.x;
	float ypos = position.y;

	float vertices[] = {
		xpos, ypos,          0.0f,  height,
		xpos,  ypos + h,     0.0f,  0.0f,
		w + xpos,  ypos + h, width, 0.0f,
		w + xpos, ypos,      width, height
	};

	unsigned int indices[] = {
		1, 0, 3,   // first triangle
		3, 2, 1    // second triangle
	};

	glUseProgram(m_screenQuadShader.ID);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
	glUniform2f(glGetUniformLocation(m_screenQuadShader.ID, "iResolution"), m_width, m_height);
	glUniform1f(glGetUniformLocation(m_screenQuadShader.ID, "iTime"), time);
	glUniformMatrix4fv(glGetUniformLocation(m_screenQuadShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
}

void TextRenderer::drawQuadTexture(Texture tex, glm::vec2 position, float width, float height, TextureAtlasPart part, glm::vec3 color) {
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(m_glyphShader.ID);
	glBindTexture(GL_TEXTURE_2D, m_fontAtlas.id);
	glBindVertexArray(m_VAO);

	float xpos = position.x;
	float ypos = position.y;

	float vertices[] = {
		// x     y
		 xpos,         ypos + height,    part.x,               part.y + part.height,
		 xpos,         ypos,             part.x,               part.y,
		 width + xpos, ypos,             part.x + part.width,  part.y,
		 width + xpos, ypos + height,    part.x + part.width,  part.y + part.height
	};

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
	glUniform3f(glGetUniformLocation(m_glyphShader.ID, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(m_glyphShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}

void TextRenderer::addTextQuad(int index, Texture tex, glm::vec2 position, float width, float height, TextureAtlasPart part, glm::vec3 color) {

	float xpos = position.x;
	float ypos = position.y;

	m_vertices.push_back(xpos);
	m_vertices.push_back(ypos + height);
	m_vertices.push_back(part.x);
	m_vertices.push_back(part.y + part.height);

	m_vertices.push_back(xpos);
	m_vertices.push_back(ypos);
	m_vertices.push_back(part.x);
	m_vertices.push_back(part.y);

	m_vertices.push_back(width + xpos);
	m_vertices.push_back(ypos);
	m_vertices.push_back(part.x + part.width);
	m_vertices.push_back(part.y);

	m_vertices.push_back(width + xpos);
	m_vertices.push_back(ypos + height);
	m_vertices.push_back(part.x + part.width);
	m_vertices.push_back(part.y + part.height);

	m_quadsIndices.push_back(0 + index*4);
	m_quadsIndices.push_back(1 + index*4);
	m_quadsIndices.push_back(3 + index*4);
	m_quadsIndices.push_back(1 + index*4);
	m_quadsIndices.push_back(2 + index*4);
	m_quadsIndices.push_back(3 + index*4);
	
}

void TextRenderer::drawText(const std::string text, glm::vec2 position, glm::vec3 color) {
	position.y += scrollAmmount;
	m_textPosition = position;
	glm::vec2 charPos = position;
	int index = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		Character ch = characters[*c];
		float w = ch.size.x;
		float h = ch.size.y;

		float xpos = charPos.x + ch.bearing.x;
		float ypos = charPos.y + (this->characters['H'].bearing.y - ch.bearing.y);

		float texturePos = float(ch.texCoord) / float(m_fontAtlas.width);
		float texw = (float(ch.texCoord + w) / float(m_fontAtlas.width)) - texturePos;
		float texh = float(ch.size.y) / float(m_fontAtlas.height);

		TextureAtlasPart part = { texturePos, 0.0, texw, texh };

		if (*c != '\n') {
			glm::vec3 lineBgColor = { 0.2, 0.2, 0.2 };
			char c = 'h';
			Character ch = characters[c];

			float cw = ch.size.x;
			float chh = ch.size.y;

			addTextQuad(index, m_fontAtlas, { xpos, ypos }, w, h, part, color);
		}

		index += 1;
		charPos.x += ch.advance >> 6;

		if (charPos.x + w >= m_width) {
			charPos.x = position.x;
			charPos.y += m_fontAtlas.height;
		}

		if (*c == '\n') {
			charPos.x = position.x;
			charPos.y += m_fontAtlas.height;
		}
	}

	auto carretPosition = getTextPositionFromIndex(m_carretIndex);

	int lines = getVisibleLinesCount();
	if (carretPosition.y >= m_height) {
		scrollAmmount = (lines - 2 - m_lineNumber) * m_fontAtlas.height;
	}

	if (carretPosition.y <= 0.0f) {
		scrollAmmount = m_lineNumber * -m_fontAtlas.height;
	}
	
	glUseProgram(m_glyphShader.ID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, m_fontAtlas.id);
	glBindVertexArray(m_VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_quadsIndices.size(), m_quadsIndices.data(), GL_STATIC_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
	glUniform3f(glGetUniformLocation(m_glyphShader.ID, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(m_glyphShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glDrawElements(GL_TRIANGLES, m_quadsIndices.size(), GL_UNSIGNED_INT, 0);

	drawCarret(carretPosition, color);
}

void Renderer::TextRenderer::setText(const std::string& text) {
	m_text = text;
	calculateTextNewLineIndices();
}

int Renderer::TextRenderer::getVisibleLinesCount() {
	return m_height / m_fontAtlas.height;
}

void Renderer::TextRenderer::calculateTextNewLineIndices() {
	m_newLineIndices.clear();
	for (int i = 0; i < m_text.size(); i++) {
		if (m_text[i] == '\n') {
			m_newLineIndices.push_back(i);
		}
	}
}

glm::vec2 Renderer::TextRenderer::getTextPositionFromIndex(int textIndex) {
	int index = 0;
	glm::vec2 carretPosition = m_textPosition;
	for (auto c = m_text.begin(); c != m_text.end(); c++) {
		if (index >= textIndex) {
			break;
		}

		Character ch = characters[*c];
		float w = ch.size.x;
		float h = ch.size.y;

		carretPosition.x += ch.advance >> 6;

		if (carretPosition.x + w >= m_width) {
			carretPosition.x = m_textPosition.x;
			carretPosition.y += m_fontAtlas.height;
		}

		if (*c == '\n') {
			carretPosition.x = m_textPosition.x;
			carretPosition.y += m_fontAtlas.height;
		}

		index++;
	}
	return carretPosition;
}

void Renderer::TextRenderer::setCarretIndex(int index) {
	m_carretIndex = index;
}

void Renderer::TextRenderer::setLineNumber(int lineNumber) {
	m_lineNumber = lineNumber;
}

void TextRenderer::drawCarret(glm::vec2 position, glm::vec3 color) {
	char c = 'h';
	Character ch = characters[c];

	float xpos = position.x + ch.bearing.x;
	float ypos = position.y + (this->characters['H'].bearing.y - ch.bearing.y);
	float w = ch.size.x;
	float h = ch.size.y;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	drawQuad({ xpos , ypos }, w, h, color);
	glDisable(GL_BLEND);
}

void TextRenderer::reloadShader() {
	glUseProgram(0);
	m_screenQuadShader = m_screenQuadShader.reloadShader("../assets/shaders/screenQuad.glsl");
	glUseProgram(m_screenQuadShader.ID);
	std::cout << "shader reloaded" << std::endl;
}

void Renderer::TextRenderer::characterInput(GLFWwindow* window, unsigned int keyCode) {
	if (m_carretIndex != m_text.size()) {
		m_text.insert(m_text.begin() + m_carretIndex, keyCode);
		m_carretIndex += 1;
	}
	else {
		m_text += keyCode;
		m_carretIndex += 1;
	}
	calculateTextNewLineIndices();
}

void Renderer::TextRenderer::keyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_BACKSPACE && action != GLFW_RELEASE) {
		if (!m_text.empty() && m_carretIndex != 0) {
			m_text.erase(m_text.begin() + (m_carretIndex - 1));
			m_carretIndex -= 1;
		}

		calculateTextNewLineIndices();
	}

	if (key == GLFW_KEY_DELETE && action != GLFW_RELEASE) {
		if (!m_text.empty() && m_carretIndex != 0) {
			m_text.erase(m_text.begin() + m_selectionStartIndex, m_text.begin() + m_selectionEndIndex);
		}
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		if (m_carretIndex != m_text.size()) {
			m_text.insert(m_text.begin() + m_carretIndex, '\n');
			m_carretIndex += 1;
			m_lineNumber += 1;
		}
		else {
			m_text += '\n';
			m_carretIndex += 1;
		}
		calculateTextNewLineIndices();
	}

	if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
		if (m_carretIndex > 0) {
			m_carretIndex -= 1;
		}
		if (!m_isSelecting) {
			m_selectionStartIndex = m_carretIndex;
			m_selectionEndIndex = m_carretIndex + 1;
		}
	}

	if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
		if (m_carretIndex < m_text.size()) {
			m_carretIndex += 1;
		}
		if (!m_isSelecting) {
			m_selectionStartIndex = m_carretIndex;
			m_selectionEndIndex = m_carretIndex + 1;
		}
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		m_selectionStartIndex = m_carretIndex;
	}

	if (mods & GLFW_MOD_SHIFT) {
		m_isSelecting = true;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		m_isSelecting = false;
		m_selectionEndIndex = m_carretIndex;
		m_selectionStartIndex = m_carretIndex;
	}

	if (mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
		if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
			m_selectionEndIndex = m_carretIndex;
		}
	}

	if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
		m_lineNumber -= 1;
		if (m_lineNumber < 0) {
			m_lineNumber = 0;
		}
		if (!m_newLineIndices.empty()) {
			m_carretIndex = m_newLineIndices[m_lineNumber];
		}
	}

	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
		if (!m_newLineIndices.empty()) {
			if (m_lineNumber < m_newLineIndices.size() - 1) {
				m_lineNumber += 1;
				m_carretIndex = m_newLineIndices[m_lineNumber];
			}
		}
	}

	if ((mods & GLFW_MOD_CONTROL)) {
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			std::ofstream out("../assets/shaders/screenQuad.glsl");
			out << m_text;
			out.close();
			printf("file saved!\n");
			reloadShader();
		}
	}
}

void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	TextRenderer* ptr = (TextRenderer*)glfwGetWindowUserPointer(window);
	ptr->characterInput(window, keyCode);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TextRenderer* ptr = (TextRenderer*)glfwGetWindowUserPointer(window);
	ptr->keyInput(window, key, scancode, action, mods);
}
