//
// Created by 777ni on 2026/09/11.
//
#include "Loaders.h"
#include <iostream>
#include <stb_image.h>
#include <tiny_gltf.h>
#include "../Meshes/Meshes.h"

// Load a texture from file using stb_image
unsigned int Loaders::loadImageFromFile(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// Load a glTF model from file
bool Loaders::loadModelFromFile(const char* path, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    if (!warn.empty()) std::cout << "Warn: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Err: " << err << std::endl;

    if (!ret) {
        std::cerr << "Failed to load glTF: " << path << std::endl;
        return false;
    }
    return true;
}

// Extract mesh data (positions + UVs + indices)
MeshData Loaders::extractMeshData(const tinygltf::Model& model, int meshIndex) {
    MeshData data;
    const tinygltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives) {
        // Positions
        const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
        const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const float* positions = reinterpret_cast<const float*>(
            &posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);

        // UVs
        const float* uvs = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];
            uvs = reinterpret_cast<const float*>(
                &uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset]);
        }

        // Fill vertex buffer
        for (size_t i = 0; i < posAccessor.count; ++i) {
            data.vertices.push_back(positions[i * 3 + 0]);
            data.vertices.push_back(positions[i * 3 + 1]);
            data.vertices.push_back(positions[i * 3 + 2]);

            if (uvs) {
                data.vertices.push_back(uvs[i * 2 + 0]);
                data.vertices.push_back(uvs[i * 2 + 1]);
            } else {
                data.vertices.push_back(0.0f);
                data.vertices.push_back(0.0f);
            }
        }

        // Indices (outside the vertex loop!)
        const tinygltf::Accessor& idxAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& idxView = model.bufferViews[idxAccessor.bufferView];
        const tinygltf::Buffer& idxBuffer = model.buffers[idxView.buffer];
        const unsigned short* indices = reinterpret_cast<const unsigned short*>(
            &idxBuffer.data[idxView.byteOffset + idxAccessor.byteOffset]);

        data.indices.insert(data.indices.end(), indices, indices + idxAccessor.count);
    }
    return data;
}

