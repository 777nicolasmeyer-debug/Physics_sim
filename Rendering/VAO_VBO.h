//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_VAO_VBO_H
#define PHYSICS_SIM_VAO_VBO_H
#include "../Meshes/Meshes.h"
#include "../Loaders/Loaders.h"
#include <glad/glad.h>

class VAO_VBO {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indexCount;
    int indexType;
    void init(const MeshData& meshData);
    void draw() const;
};

#endif //PHYSICS_SIM_VAO_VBO_H
