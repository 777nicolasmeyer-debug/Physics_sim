//
// Created by 777ni on 2026/09/14.
//

#ifndef PHYSICS_SIM_OBB_H
#define PHYSICS_SIM_OBB_H
#include "glm/glm.hpp"
enum class ColliderType {
    Box,
    Plane
};

class OBB {
public:
    glm::vec3 center;
    glm::vec3 halfSize;

    glm::vec3 right{1.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 forward{0.0f, 0.0f, 1.0f};

    ColliderType type;
};

#endif //PHYSICS_SIM_OBB_H
