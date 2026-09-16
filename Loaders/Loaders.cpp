//
// Created by 777ni on 2026/09/11.
//
#define STB_IMAGE_IMPLEMENTATION
#include "Loaders.h"

#include <iostream>
#include <ostream>

unsigned int Loaders::loadImageFromFile(const char *path) {
    int width, height, channels;
    data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return texture;
}

bool Loaders::loadModelFromFile(const char* path, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = false;

    if (std::string(path).find(".glb") != std::string::npos) {
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    }
    else {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }

    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load model: " << path << std::endl;
        return false;
    }

    return true;
}

MeshData Loaders::extractMeshData(const tinygltf::Model& model, int meshIndex) {
    MeshData data;
    const tinygltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives) {
        const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
        const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);

        const float* uvs = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];
            uvs = reinterpret_cast<const float*>(&uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset]);
        }

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
            const tinygltf::Accessor& idxAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& idxView = model.bufferViews[idxAccessor.bufferView];
            const tinygltf::Buffer& idxBuffer = model.buffers[idxView.buffer];
            const unsigned short* indices = reinterpret_cast<const unsigned short*>(&idxBuffer.data[idxView.byteOffset + idxAccessor.byteOffset]);
            data.indices.insert(data.indices.end(), indices, indices + idxAccessor.count);
        }
    }
    return data;
}
