//
// Created by 777ni on 2026/09/14.
//

#ifndef PHYSICS_SIM_GRAVITY_H
#define PHYSICS_SIM_GRAVITY_H

#include "glad/glad.h"
#include "glm/glm.hpp"

class Gravity {
public:
    void apply(glm::vec3& velocity, const float deltaTime);
private:
    const float GravStrength = 9.8f;
};


#endif //PHYSICS_SIM_GRAVITY_H
