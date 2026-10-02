//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_SHADER_H
#define PHYSICS_SIM_SHADER_H
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/fwd.hpp>

class Shader {
public:
    GLuint ID;

    void createShaders(const char* vertex_path, const char* fragment_path);
    void use();
    void loadMatrix(const char * name, const glm::mat4 & matrix);
    void loadVector3(const char * name, const glm::vec3 & vector);
    void loadVector4(const char * name, const glm::vec4 & vector);
    void loadTexture(const char* name, int textureUnit);

};
#endif //PHYSICS_SIM_SHADER_H
