//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_MESHES_H
#define PHYSICS_SIM_MESHES_H
#include <glm/glm.hpp>
#include <vector>

namespace Meshes {
    inline constexpr std::array<float, 180> CubeVertices = {
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
    struct Cube {
        glm::vec3 position;
        glm::vec3 velocity;

        Cube();
    };

    inline constexpr std::array<float, 180> PlaneVertices = {
        // Bottom face (y = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f, -0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f, -0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Top face (y = +0.5)
    -0.5f,  0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f,  0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f,  0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Front face (z = +0.5)
    -0.5f, -0.1f,  0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Back face (z = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f, -0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    // Left face (x = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
    -0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    -0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    // Right face (x = +0.5)
     0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
     0.5f,  0.1f, -0.5f, 0.0f, 1.0f  // top left
    };
    struct Plane {
        glm::vec3 position;
        Plane();
    };

    inline constexpr std::array<float, 360> allVertices = {
    // CubeVertices
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
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  // top left

     // Plane Vertices
        // Bottom face (y = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f, -0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f, -0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Top face (y = +0.5)
    -0.5f,  0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f,  0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f,  0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Front face (z = +0.5)
    -0.5f, -0.1f,  0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f,  0.5f, 0.0f, 1.0f, // top left

    // Back face (z = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f, -0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f, -0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    // Left face (x = -0.5)
    -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
    -0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    -0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
    -0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
    -0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

    // Right face (x = +0.5)
     0.5f, -0.1f, -0.5f, 0.0f, 0.0f, // bottom left
     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f, -0.5f, 0.0f, 1.0f, // top left

     0.5f, -0.1f,  0.5f, 1.0f, 0.0f, // bottom right
     0.5f,  0.1f,  0.5f, 1.0f, 1.0f, // top right
     0.5f,  0.1f, -0.5f, 0.0f, 1.0f,  // top left
    };
}
#endif //PHYSICS_SIM_MESHES_H
