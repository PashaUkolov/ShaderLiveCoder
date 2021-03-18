#include "TextRenderer.hpp"

TextRenderer::TextRenderer() {
    // FreeType init
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
	printf("an error occuret duting fritype initializaton \n");
    }
}

TextRenderer::~TextRenderer() {
    
}

void TextRenderer::loadFont(const std::string& path, unsigned int fontSize) {
    FT_Library lib;
    FT_Face face;

    FT_Error error = FT_Init_FreeType(&lib);

    if ( error ) {
	printf("an error occuret duting fritype initializaton \n");
    }

    error = FT_New_Face(lib, path.c_str(), 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
	printf("font format is unsupported");
	    } else if ( error ) {
	printf("font file couldn't be opened or read, or it is broken...");

    }
}

void TextRenderer::renderText() {
    unsigned int VBO;
    unsigned int VAO;

    //Shader shader;
    //glUseProgram(shader);
}
