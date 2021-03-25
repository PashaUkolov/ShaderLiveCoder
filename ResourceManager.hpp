#include <string>
#include <map>
#include "Shader.hpp"

class ResourceManager {
    ResourceManager();
    static Shader loadShader(const char* shaderFile, const std::string& name);
};
