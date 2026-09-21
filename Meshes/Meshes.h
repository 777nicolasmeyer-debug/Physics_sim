//
// Created by 777ni on 2026/09/10.
//

#ifndef PHYSICS_SIM_MESHES_H
#define PHYSICS_SIM_MESHES_H
#include <glm/glm.hpp>
#include <vector>
#include <BulletDynamics/Dynamics/btRigidBody.h>


struct MeshData {
 std::vector<float> vertices;
 std::vector<unsigned int> indices;
 int indexType;
};
#endif //PHYSICS_SIM_MESHES_H
