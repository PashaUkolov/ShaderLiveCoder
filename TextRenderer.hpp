#include <stdio.h>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    void loadFont(const std::string& path, unsigned int size);
private:
    int var { 10 };
};
    
