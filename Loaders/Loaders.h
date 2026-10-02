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
    Loaders();
    unsigned int loadImageFromFile(const char* path);
    unsigned int texture;

    void loadModelFromFile(const char* path, tinygltf::Model& model);
    GLuint createTextureFromImage(const tinygltf::Image& image);
    bool PrimitiveCount = false;

    MeshData extractMeshData(const tinygltf::Model& model, int meshIndex);
private:
    unsigned char* data;
    GLuint createWhiteTexture();
    GLuint whiteTexture;
};

#endif //PHYSICS_SIM_LOADERS_H
