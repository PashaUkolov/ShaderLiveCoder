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

    auto error = FT_Init_FreeType(&lib);

    if ( error ) {
	printf("an error occuret duting fritype initializaton \n");
    }

    error = FT_New_Face(lib, path.c_str(), 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
	printf("... the font file could be opened and read, but it appears that its font format is unsupported");
	    } else if ( error ) {
	printf("font file could not be opened or read, or that it is broken...");

    }
}
