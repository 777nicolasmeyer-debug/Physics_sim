//
// Created by 777ni on 2026/09/11.
//

#ifndef PHYSICS_SIM_LOADERS_H
#define PHYSICS_SIM_LOADERS_H
#include "stb_image.h"
#include <glad/glad.h>
#include "../Meshes/Meshes.h"
#include <tiny_gltf.h>

class Loaders {
public:
    unsigned int loadImageFromFile(const char* path);
    unsigned int texture;

    bool loadModelFromFile(const char* path, tinygltf::Model& model);

    MeshData extractMeshData(const tinygltf::Model& model, int meshIndex);
private:
    unsigned char* data;
};

#endif //PHYSICS_SIM_LOADERS_H
