//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_VAO_VBO_H
#define PHYSICS_SIM_VAO_VBO_H
#include "../Meshes/Meshes.h"
#include <glad/glad.h>

class VAO_VBO {
public:
    GLuint vbo;
    GLuint vao;
    void createVAO();
    void createVBO();
};

#endif //PHYSICS_SIM_VAO_VBO_H
