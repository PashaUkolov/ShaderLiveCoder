#include "Shader.hpp"

Shader Shader::parseShader(std::string filePath) {
    std::stringstream vertex, fragment;
    
    std::ifstream file { filePath };
    std::string line;
    bool isVertex = false;
    while(getline(file, line)) {
	if (line.find("#shader vertex") != std::string::npos) {
	    isVertex = true;
	    continue;
	} else if (line.find("#shader fragment") != std::string::npos) {
	    isVertex = false;
	    continue;
	}

	if (isVertex) {
	    vertex << line << "\n";
	} else {
	    fragment << line << "\n";
	}
    }
    
    Shader shader;
    shader.compileShader(vertex.str().c_str(), fragment.str().c_str());
    return shader;
}

void Shader::compileShader(const char* vertexSource, const char* fragmentSource) {    
    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    checkErrors(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    checkErrors(fragment, "FRAGMENT");
    
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    
    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::checkErrors(unsigned int object, std::string type) {
    int success;
    char infoLog[2048];

    if (type != "PROGRAM") {
	glGetProgramiv(object, GL_COMPILE_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(object, 2048, NULL, infoLog);
	    std::cout << "ERROR::SHADER::compile time error Type:\n" << type
		      << "\n" << infoLog << std::endl;
	}
    } else {
	glGetProgramiv(object, GL_LINK_STATUS, &success);
	if(!success) {
	    glGetShaderInfoLog(ID, 2048, NULL, infoLog);
	    std::cout << "ERROR::SHADER::link error Type:\n" << type
		      << "\n" << infoLog << std::endl;
	} 
    }    
}

Shader Shader::reloadShader(std::string filePath) {
    Shader shader;
    shader = parseShader(filePath);
    return shader;
}
