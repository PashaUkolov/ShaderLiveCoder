#include "TextRenderer.hpp"

TextRenderer::TextRenderer(int screenWidth, int screenHeight) :
    width(screenWidth), height(screenHeight)   {
    
    //opengl stupid steps
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

    float vertices [] = {
	//x    y     z
	0.5f, 0.5f, 0.0f, //top right
	0.5f, -0.5f, 0.0f, //bottom right
	-0.5f, -0.5f, 0.0f, //bottom left
	-0.5f, 0.5f, 0.0f //top left
    };

    unsigned int indeces [] = {
	0, 1, 3, //first triangle
	1, 2, 3	
    };

    // unsigned int indeces [] = {
    // 	0, 1, 2, //first triangle
    // 	0, 2, 3	
    // };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    // VBO Buffer 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
		 sizeof(float) * 6 * 4,
		 vertices,
		 GL_DYNAMIC_DRAW);
    
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
    shader = shader.parseShader("/Users/pashaukolov/Documents/Code/TextRenderer/shaders/glyph.glsl");
    glUseProgram(shader.ID);
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
    
    if (error == FT_Err_Unknown_File_Format) 
	printf("font format is unsupported");
    else if ( error ) 
	printf("font file couldn't be opened or read, or it is broken...");

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);;
    
    //int textureWidth = 0;
    //textureHeight = 0;

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

	printf("char: %c, texture width: %d \n", c, textureWidth);
	    
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
	FT_Bitmap * bmp = &face->glyph->bitmap;

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
    glUseProgram(shader.ID);
    
    time += delta;

    float s = 1.0f / 128.0f;

    float pos = std::sin(time);
    int index = 0;
    glBindTexture(GL_TEXTURE_2D, texture);

    float pad = 10.5f;
    
    for (auto c = text.begin(); c != text.end(); c++) {
	Character ch = characters[*c];
	//float xPos = index * 50.0f;

	float xpos = position.x + ch.bearing.x;
        float ypos = position.y + (this->characters['H'].bearing.y - ch.bearing.y);
	float w = ch.size.x;
        float h = ch.size.y;

	//float xPos = ch.bearing.x;
	float num = (sin(time) * 0.5f + 0.5f);
	float texturePos = float(ch.texCoord) / float(textureWidth);
	float texw = (float(ch.texCoord + w) / float(textureWidth)) - texturePos;

	float texh = float(ch.size.y) / float(textureHeight);
	   
	//float texw = 1.0f / w; 
	 // printf("%d texture pos \n", ch.texCoord);
	 // printf("%f char width \n", w);
	 // printf("%d textureWidth \n", textureWidth);
	 // printf("%f texturePOS \n", texturePos);

	//this is for testing
	// float vertices[4][4] = {
	//     // x     y
	//     { xpos,     h,    texturePos + w, 1.0f }, 
	//     { xpos,     0.0f, texturePos + w, 0.0f },
	//     { w + xpos, 0.0f, texturePos, 0.0f },
	//     { w + xpos, h,    texturePos, 1.0f },
	// };

	float offset = num;

	// float vertices[4][4] = {
	//     // x     y
	//     { xpos,     h,    texturePos, 1.0f }, 
	//     { xpos,     0.0f, texturePos, 0.0f },
	//     { w + xpos, 0.0f, texturePos + texw, 0.0f },
	//     { w + xpos, h,    texturePos + texw, 1.0f },
	// };

	float vertices[4][4] = {
	    // x     y
	    { xpos,     ypos + h,    texturePos,        texh }, 
	    { xpos,     ypos,        texturePos,        0.0f },
	    { w + xpos, ypos,        texturePos + texw, 0.0f },
	    { w + xpos, ypos + h,    texturePos + texw, texh },
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	position.x += ch.advance >> 6;
	index++;
    }

    //glBindVertexArray(VAO);
    //glBindTexture(GL_TEXTURE_2D, texture);
    // glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glm::vec2 size = glm::vec2(5000.0f, 100.0f);
    //glm::vec2 position = glm::vec2(mouse.x, mouse.y);

    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
        
    //all uniforms
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(projection));
    
    glUniform1i(glGetUniformLocation(this->shader.ID, "fontTexture"), 0);
    //glUniform1f(glGetUniformLocation(shader.ID, "time"), time);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void TextRenderer::reloadShader() {
    shader = shader.reloadShader("/Users/pashaukolov/Documents/Code/TextRenderer/shaders/glyph.glsl");
    glUseProgram(shader.ID);
    std::cout << "shader reloaded" << std::endl;
}
