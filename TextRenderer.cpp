#include "TextRenderer.hpp"

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

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

    glVertexAttribPointer(0, 3, GL_FLOAT,
			  GL_FALSE,
			  3 * sizeof(float), (GLvoid*)0);

    glEnableVertexAttribArray(0);

    //glBindVertexArray(0); 
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
    
    int textureWidth = 0;
    int textureHeight = 0;

    for (GLubyte c = 0; c < 128; c++) {
	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
	    printf("ERROR::FREETYTPE: Failed to load Glyph \n");

	int height = face->glyph->bitmap.rows;

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

void TextRenderer::drawText(std::string text, float delta, glm::vec2 mouse, glm::vec3 color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shader.ID);
    
    time += delta;

    float vertices[6][4] = {
	{ 0.0f, 1.0f,   0.0f, 1.0f },
	{ 1.0f, 0.0f,   1.0f, 0.0f },
	{ 0.0f, 0.0f,   0.0f, 0.0f },
            
	{ 0.0f, 1.0f,   0.0f, 1.0f },
	{ 1.0f, 1.0f,   1.0f, 1.0f },
	{ 1.0f, 0.0f,   1.0f, 0.0f }
    };

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glm::vec2 size = glm::vec2(5000.0f, 100.0f);
    glm::vec2 position = glm::vec2(mouse.x, mouse.y);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
        
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(-0.0f * size.x, -0.0f * size.y, 0.0f));     
    model = glm::scale(model, glm::vec3(size, 1.0f)); 

    //all uniforms
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glUniform2f(glGetUniformLocation(shader.ID, "size"), width, height);
    glUniform2f(glGetUniformLocation(shader.ID, "position"), mouse.x, mouse.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, false, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(this->shader.ID, "fontTexture"), 0);
    glUniform1f(glGetUniformLocation(shader.ID, "time"), time);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void TextRenderer::reloadShader() {
    shader = shader.reloadShader("/Users/pashaukolov/Documents/Code/TextRenderer/shaders/glyph.glsl");
    glUseProgram(shader.ID);
    std::cout << "shader reloaded" << std::endl;
    
}
