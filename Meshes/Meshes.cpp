//
// Created by 777ni on 2026/09/10.
//
#include "Meshes.h"

#include <GLFW/glfw3.h>
#include <glm/ext/scalar_constants.hpp>

Meshes::Cube::Cube() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    angularVel = glm::vec3(0.0f, 0.0f, 0.0f);
    collider.halfSize = glm::vec3(0.5f, 0.5f, 0.5f);
};

Meshes::Plane::Plane() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    collider.halfSize = glm::vec3(10.0f, 0.1f, 10.0f);
}
