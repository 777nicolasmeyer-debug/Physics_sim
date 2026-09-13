//
// Created by 777ni on 2026/09/10.
//
#include "VAO_VBO.h"

void VAO_VBO::setCubeVBO(Meshes::Cube& cube) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, cube.vertices.size() * sizeof(float), cube.vertices.data(), GL_STATIC_DRAW);
}

void VAO_VBO::setCubeVAO(Meshes::Cube &cube) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}
