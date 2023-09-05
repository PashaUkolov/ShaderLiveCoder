#include "TextRenderer.h"
using namespace Renderer;

TextRenderer::TextRenderer(GLFWwindow* window, int screenWidth, int screenHeight) :
	m_window(window),
	m_width(screenWidth),
	m_height(screenHeight),
	m_inputHandler(window)
{
	for (int i = 0; i < m_text.size(); i++) {
		if (m_text[i] == '\n') {
			newLineIndices.push_back(i);
		}
	}
	m_inputHandler.init();
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

void TextRenderer::init() {
	float w = 100.0;
	float h = 100.0;
	float xpos = 0.0f;
	float ypos = 0.0f;
	float vertices[] = {
		//pos xy            texture xy 
		xpos,     ypos + h, 0.0f, 0.0f, // top right
		xpos,     ypos,     0.0f, 0.0f, // bottom right
		w + xpos, ypos,     0.0f, 0.0f, // bottom left
		w + xpos, ypos + h, 0.0f, 0.0f // top left 
	};

	unsigned int indeces[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	m_glyphShader = m_glyphShader.parseShader("../assets/shaders/glyph.glsl");
	m_quadShader = m_quadShader.parseShader("../assets/shaders/quad.glsl");
	m_screenQuadShader = m_screenQuadShader.parseShader("../assets/shaders/screenQuad.glsl");

	glUseProgram(m_glyphShader.ID);
	glUseProgram(m_quadShader.ID);
	glUseProgram(m_screenQuadShader.ID);

	GLint location = glGetUniformLocation(m_screenQuadShader.ID, "iResolution");
	if (location != -1) {
		glProgramUniform2f(m_screenQuadShader.ID, location, m_width, m_height);
	}
}

void TextRenderer::beginFrame() {
	glClearColor(0.0f, 0.05f, 0.08f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TextRenderer::endFrame() {
	glfwSwapBuffers(m_window);
	glfwPollEvents();
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

void TextRenderer::drawScreenQuad(glm::vec2 position, float width, float height, glm::vec3 color) {
	float w = width;
	float h = height;
	float xpos = position.x;
	float ypos = position.y;
	float vertices[] = {
		xpos,  ypos + h,     0.0f, 1.0f,  // top right
		xpos, ypos,          0.0f, 0.0f,  // bottom right
		w + xpos, ypos,      1.0f, 0.0f,  // bottom left
		w + xpos,  ypos + h, 1.0f, 1.0f   // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glUseProgram(m_screenQuadShader.ID);
	glBindVertexArray(m_VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
	glUniform3f(glGetUniformLocation(m_screenQuadShader.ID, "quadColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(m_screenQuadShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
}

void TextRenderer::drawQuadTexture(Texture tex, glm::vec2 position, float width, float height, TextureAtlasPart part, glm::vec3 color) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void TextRenderer::drawText(const std::string& text, glm::vec2 position, glm::vec3 color) {
	glm::vec2 charPos = position - scrollAmmount;
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
			drawQuadTexture(m_fontAtlas, { xpos, ypos }, w, h, part, color);
		}

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

	int index = 0;
	glm::vec2 carretPosition = position;
	for (auto c = text.begin(); c != text.end(); c++) {
		if (index >= m_carretIndex) {
			break;
		}

		int lines = getVisibleLinesCount();

		if (carretPosition.y + m_fontAtlas.height >= m_height) {
			scrollAmmount += m_fontAtlas.height;
			carretPosition.y -= (m_fontAtlas.height * 2);
			int a = 0;
			//carretPosition.y = carretPosition.y;
			break;
			//scrollAmmount = m_fontAtlas.height;
		}

		Character ch = characters[*c];
		float w = ch.size.x;
		float h = ch.size.y;

		carretPosition.x += ch.advance >> 6;

		if (carretPosition.x + w >= m_width) {
			carretPosition.x = position.x;
			carretPosition.y += m_fontAtlas.height;
		}

		if (*c == '\n') {
			carretPosition.x = position.x;
			carretPosition.y += m_fontAtlas.height;
		}
		index++;
	}

	drawCarret(carretPosition, color, 0.0f);
}

void Renderer::TextRenderer::setText(const std::string& text) {
	m_text = text;
}

int Renderer::TextRenderer::getVisibleLinesCount() {
	return m_height / m_fontAtlas.height;
}

glm::vec2 Renderer::TextRenderer::getCarrentPositionFromIndex(int index) {
	return {};
}

void Renderer::TextRenderer::setCarretIndex(int index) {
	m_carretIndex = index;
}

void Renderer::TextRenderer::setLineNumber(int lineNumber) {
	m_lineNumber = lineNumber;
}

void TextRenderer::drawCarret(glm::vec2 position, glm::vec3 color, float time) {
	float animation = std::sin(time);

	char c = 'h';
	Character ch = characters[c];

	float xpos = position.x + ch.bearing.x;
	float ypos = position.y + (this->characters['H'].bearing.y - ch.bearing.y);
	float w = ch.size.x;
	float h = ch.size.y;

	float num = (sin(time) * 0.5f + 0.5f);
	float texturePos = float(ch.texCoord) / float(m_fontAtlas.width);
	float texw = (float(ch.texCoord + w) / float(m_fontAtlas.width)) - texturePos;
	float texh = float(ch.size.y) / float(m_fontAtlas.height);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	drawQuad({ xpos , ypos }, w, h, color);
}

void TextRenderer::reloadShader() {
	m_glyphShader = m_glyphShader.reloadShader("../assets/shaders/glyph.glsl");
	glUseProgram(m_glyphShader.ID);
	std::cout << "shader reloaded" << std::endl;
}
