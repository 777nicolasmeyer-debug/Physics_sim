//
// Created by 777ni on 2026/09/14.
//

#include "gravity.h"

void Gravity::apply(glm::vec3& velocity, const float deltaTime) {
    velocity.y -= GravStrength * deltaTime;
}