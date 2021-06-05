#pragma once

#include <stdio.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.hpp"

class TextRenderer {
public:
    TextRenderer(int screenWidth, int screenHeight);
    ~TextRenderer();
    void bindTexture();
    void loadFont(const std::string& path, unsigned int size);

    void renderText(std::string text, float delta, glm::vec2 mouse, glm::vec3 color);
    void reloadShader();
private:
    float width, height;
    Shader shader;
    unsigned int VBO, VAO, EBO;
    float time = 0;
    unsigned int texture;
};
    
