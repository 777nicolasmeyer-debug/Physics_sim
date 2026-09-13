//
// Created by 777ni on 2026/09/10.
//
#include "../Meshes/Shader.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void Shader::createShaders(const char* vertex_path, const char* fragment_path) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::badbit|std::ifstream::failbit);
    fShaderFile.exceptions(std::ifstream::badbit|std::ifstream::failbit);

    try {

        vShaderFile.open(vertex_path);
        fShaderFile.open(fragment_path);

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "Failed to read Shader Files" << std::endl;
    }
    const char* vSource = vertexCode.c_str();
    const char* fSource = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vSource, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Vertex shader compilation failed" << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Fragment shader compilation failed" << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Program linking failed" << std::endl;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::loadMatrix(const char *name, const glm::mat4 &matrix) {
    glGetUniformLocation(ID, name);
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::loadTexture(const char* name, int textureunit) {
    unsigned int location = glGetUniformLocation(ID, name);
    glUniform1i(location, textureunit);
}
