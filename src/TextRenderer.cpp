#include "TextRenderer.h"

TextRenderer::TextRenderer(int screenWidth, int screenHeight) :
	width(screenWidth), height(screenHeight) {

	//opengl steps
	//1. create vertex coorinates
	//2. Generate vbo buffer with glGenBuffer
	//3. Bind this buffer to vbo
	//4. Copy vertecies to vertex buffer with glBufferData
	//5. Store all that stuff in VAO for later access
	//6. specify attribPointer and enable attribArray
	//7. can specify getUniform3f for setting fragment shader colour
	//8. specify in fragment layout location and gl_position, this tells
	// to the glsl location of the vertices
	//9. plug a EBO for drawing quad,
	// so that you dont need to process dublicate verteces
	//10. transform screen coordinates space to object space by projecting

	float vertices[] = {
		//x    y     z
		0.5f, 0.5f, 0.0f, 0.0f, //top right
		0.5f, -0.5f, 0.0f, 0.0f, //bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, //bottom left
		-0.5f, 0.5f, 0.0f, 0.0f //top left
	};

	const size_t maxQuadCount = 5000;
	const size_t maxVertexCount = maxQuadCount * 4;
	const size_t maxIndexCount = maxQuadCount * 6;

	// unsigned int indeces [] = {
	// 	0, 1, 3, //first triangle
	// 	1, 2, 3,
	// 	4, 5, 7, //first triangle
	// 	5, 6, 7	
	// };

	uint32_t indeces[maxIndexCount];

	uint32_t offset = 0;
	for (size_t i = 0; i < maxIndexCount; i += 6) {
		indeces[i + 0] = 0 + offset;
		indeces[i + 1] = 1 + offset;
		indeces[i + 2] = 3 + offset;

		indeces[i + 3] = 1 + offset;
		indeces[i + 4] = 2 + offset;
		indeces[i + 5] = 3 + offset;

		offset += 4;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// VBO Buffer 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		(sizeof(float) * 4) * maxVertexCount,
		nullptr,
		GL_STATIC_DRAW);

	// EBO Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indeces),
		indeces,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float), (GLvoid*)0);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glyphShader = glyphShader.parseShader("../assets/shaders/glyph.glsl");
	carretShader = carretShader.parseShader("../assets/shaders/carret.glsl");

	glUseProgram(glyphShader.ID);
	glUseProgram(carretShader.ID);
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
			textureWidth
		};

		characters.insert(std::pair<char, Character>(c, character));

		//get highest char
		if (height > textureHeight)
			textureHeight = height;

		textureWidth += face->glyph->bitmap.width;
	}

	unsigned char* map = (unsigned char*)calloc(textureWidth * textureHeight, 1);

	int offset = 0;

	for (GLubyte c = 0; c < 128; c++) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		FT_Bitmap* bmp = &face->glyph->bitmap;

		int rows = bmp->rows;
		int width = bmp->width;

		for (int row = 0; row < rows; ++row) {
			for (int i = 0; i < width; ++i) {
				map[row * textureWidth + offset + i] = bmp->buffer[row * bmp->pitch + i];
			}
		}

		offset += width;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RED,
		textureWidth,
		textureHeight,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		map);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

void TextRenderer::drawText(std::string text, float delta, glm::vec2 position, glm::vec3 color) {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(glyphShader.ID);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	time += delta;
	int min = 0;
	int max = 4;

	size_t index = 0;
	uint32_t indexCount = 0;

	for (auto c = text.begin(); c != text.end(); c++) {
		Character ch = characters[*c];
		// int rand =  min + (std::rand() % static_cast<int>(max - min + 1));
		// Character ch = characters[int(time + rand) % 128];
		float w = ch.size.x;
		float h = ch.size.y;

		if (position.x + w >= width) {
			position.x = 10.0f;
			position.y += textureHeight;
		}

		float xpos = position.x + ch.bearing.x;
		float ypos = position.y + (this->characters['H'].bearing.y - ch.bearing.y);

		float texturePos = float(ch.texCoord) / float(textureWidth);
		float texw = (float(ch.texCoord + w) / float(textureWidth)) - texturePos;
		float texh = float(ch.size.y) / float(textureHeight);

		float vertices[] = {
			// x     y
			 xpos,     ypos + h,    texturePos,        texh ,
			 xpos,     ypos,        texturePos,        0.0f ,
			 w + xpos, ypos,        texturePos + texw, 0.0f ,
			 w + xpos, ypos + h,    texturePos + texw, texh ,
		};

		glBufferSubData(GL_ARRAY_BUFFER,
			index * sizeof(vertices),
			sizeof(vertices),
			vertices);

		position.x += ch.advance >> 6;

		if (*c == '\n') {
			position.x = 10.0f;
			position.y += textureHeight;
		}

		indexCount += 6;
		index++;
	}
	// glBindVertexArray(VAO);
	// glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	//all uniforms
	glUniform3f(glGetUniformLocation(glyphShader.ID, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(glyphShader.ID, "projection"), 1, false, glm::value_ptr(projection));

	glUniform1i(glGetUniformLocation(glyphShader.ID, "fontTexture"), 0);

	drawCarret(position, color, time);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void TextRenderer::drawCarret(glm::vec2 position, glm::vec3 color, float time) {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(carretShader.ID);

	float animation = std::sin(time);

	char c = 'h';
	Character ch = characters[c];

	float xpos = position.x + ch.bearing.x;
	float ypos = position.y + (this->characters['H'].bearing.y - ch.bearing.y);
	float w = ch.size.x;
	float h = ch.size.y;

	float num = (sin(time) * 0.5f + 0.5f);
	float texturePos = float(ch.texCoord) / float(textureWidth);
	float texw = (float(ch.texCoord + w) / float(textureWidth)) - texturePos;

	float texh = float(ch.size.y) / float(textureHeight);

	float vertices[4][4] = {
		// x     y
		{ xpos,     ypos + h,    texturePos,        texh },
		{ xpos,     ypos,        texturePos,        0.0f },
		{ w + xpos, ypos,        texturePos + texw, 0.0f },
		{ w + xpos, ypos + h,    texturePos + texw, texh },
	};

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	position.x += ch.advance >> 6;

	glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	//all uniforms
	glUniform3f(glGetUniformLocation(carretShader.ID, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(carretShader.ID, "projection"), 1, false, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(carretShader.ID, "time"), time);

	// glBindVertexArray(0);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// glDisable(GL_BLEND);
}

void TextRenderer::reloadShader() {
	glyphShader = glyphShader.reloadShader("../assets/shaders/glyph.glsl");
	glUseProgram(glyphShader.ID);
	std::cout << "shader reloaded" << std::endl;
}
