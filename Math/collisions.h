//
// Created by 777ni on 2026/09/14.
//

#ifndef PHYSICS_SIM_COLLISIONS_H
#define PHYSICS_SIM_COLLISIONS_H
#include "../Math/OBB.h"
#include "../Meshes/Meshes.h"
#include "glm/glm.hpp"
#include <algorithm>

class Collisions {
public:
    bool BoxBoxCollision(OBB& box1, OBB& box2);;
    void ResolveBoxBoxCollision(OBB &box1, OBB &box2, float overlap, const glm::vec3 &normal);;
};


#endif //PHYSICS_SIM_COLLISIONS_H
