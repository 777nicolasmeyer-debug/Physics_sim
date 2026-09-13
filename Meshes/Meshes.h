//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_MESHES_H
#define PHYSICS_SIM_MESHES_H
#include <glm/glm.hpp>
#include <vector>

namespace Meshes {
    struct Cube {
        std::vector<float> vertices;
        glm::vec3 position;
        glm::vec3 velocity;

        Cube();
    };
}
#endif //PHYSICS_SIM_MESHES_H
