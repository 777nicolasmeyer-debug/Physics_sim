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
    void removeRigidBody(btRigidBody* body);
    bool isGrounded(const btRigidBody* body) const;
    glm::vec3 getGroundNormal(const btRigidBody* body) const;
    btRigidBody* convexShapeD(SceneObject& sceneObj, const MeshData& mesh,
                              const glm::vec3& scale = glm::vec3(1.0f));
    btRigidBody* boxShapeD(SceneObject& sceneObj, const MeshData& mesh,
                           const glm::vec3& scale = glm::vec3(1.0f));
    btRigidBody* triangleShapeS(SceneObject& sceneObj, const MeshData& mesh);
};


#endif //PHYSICS_SIM_COLLISIONS_H
