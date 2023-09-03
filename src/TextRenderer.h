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

class TextRenderer {
public:
    TextRenderer(int screenWidth, int screenHeight);
    ~TextRenderer();
    void loadFont(const std::string& path, unsigned int size);

    void drawText(std::string text, float delta, glm::vec2 position, glm::vec3 color);

    void drawCarret(glm::vec2 position, glm::vec3 color, float time);
    void reloadShader();
    std::unordered_map<char, Character> characters;
private:
    float width, height;
    Shader glyphShader;
    Shader carretShader;
    unsigned int VBO, VAO, EBO;
    float time = 0;
    unsigned int texture;
    int textureWidth = 0;
    int textureHeight = 0;
};
    
