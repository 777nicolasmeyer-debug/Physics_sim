//
// Created by 777ni on 2026/09/15.
//

#ifndef PHYSICS_SIM_COLLISIONS_H
#define PHYSICS_SIM_COLLISIONS_H
#include <glm/glm.hpp>
#include <bullet/btBulletCollisionCommon.h>
#include "../Meshes/Meshes.h"
#include"../Scene/SceneManager.h"
#include <iostream>

class Collisions {
public:
    void init();
    void update(float dt);
    btRigidBody* convexShapeD(SceneObject& sceneObj, const MeshData& mesh);
};


#endif //PHYSICS_SIM_COLLISIONS_H
