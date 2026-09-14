//
// Created by 777ni on 2026/09/10.
//
#include "Meshes.h"

Meshes::Cube::Cube() {
    vertices = {
    // Bottom face (y = -0.5)
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, // top left

    // Top face (y = +0.5)
    -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f,  0.5f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top left

     0.5f,  0.5f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top left

    // Front face (z = +0.5)
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top left

    // Back face (z = -0.5)
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left

    // Left face (x = -0.5)
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom left
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left

    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left

    // Right face (x = +0.5)
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f  // top left
};

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
};
