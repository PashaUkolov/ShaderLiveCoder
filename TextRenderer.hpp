#include <stdio.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    void loadFont(const std::string& path, unsigned int size);

    void renderText();
private:
    int var { 10 };
};
    
