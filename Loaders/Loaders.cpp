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

void Loaders::loadModelFromFile(const char* path, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = false;

    // Decide whether to load binary (.glb) or ASCII (.gltf)
    std::string filePath(path);
    if (filePath.size() >= 5 && filePath.substr(filePath.size() - 5) == ".gltf") {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    } else {
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    }

    if (!warn.empty()) {
        std::cout << "TinyGLTF warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "TinyGLTF error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load model: " << path << std::endl;
    }

    std::cout << "Successfully loaded model: " << path << std::endl;
}

// Load a glTF model from file
// Extract mesh data (positions + normals + UVs + indices)
MeshData Loaders::extractMeshData(const tinygltf::Model& model, int meshIndex) {
    MeshData data;
    const tinygltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives) {
        // POSITION
        const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
        const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const float* positions = reinterpret_cast<const float*>(
            &posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);

        // NORMAL
        const float* normals = nullptr;
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
            const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
            const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];
            normals = reinterpret_cast<const float*>(
                &normBuffer.data[normView.byteOffset + normAccessor.byteOffset]);
        }

        // TEXCOORD_0
        const float* uvs = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];
            uvs = reinterpret_cast<const float*>(
                &uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset]);
        }
        const tinygltf::Accessor* normAccessor = nullptr;
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
            normAccessor = &model.accessors[primitive.attributes.at("NORMAL")];
        }
        const tinygltf::Accessor* uvAccessor = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            uvAccessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
        }


        // INDICES
        const tinygltf::Accessor& idxAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& idxView = model.bufferViews[idxAccessor.bufferView];
        const tinygltf::Buffer& idxBuffer = model.buffers[idxView.buffer];
        const unsigned char* base = &idxBuffer.data[idxView.byteOffset + idxAccessor.byteOffset];


        // Parse indices according to type
        std::vector<unsigned int> indices;
        if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const unsigned short* idx = reinterpret_cast<const unsigned short*>(base);
            indices.assign(idx, idx + idxAccessor.count);
        } else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            const unsigned int* idx = reinterpret_cast<const unsigned int*>(base);
            indices.assign(idx, idx + idxAccessor.count);
        } else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            const unsigned char* idx = reinterpret_cast<const unsigned char*>(base);
            indices.assign(idx, idx + idxAccessor.count);
        }

        // Build vertex buffer per index
        for (size_t i = 0; i < indices.size(); ++i) {
            unsigned int idx = indices[i];
            if (idx >= posAccessor.count) {
                std::cerr << "Index " << idx << " out of range for positions (count=" << posAccessor.count << ")" << std::endl;
                continue;
            }


            // Position
            data.vertices.push_back(positions[idx * 3 + 0]);
            data.vertices.push_back(positions[idx * 3 + 1]);
            data.vertices.push_back(positions[idx * 3 + 2]);

            // Normal
            if (normals) {
                data.vertices.push_back(normals[idx * 3 + 0]);
                data.vertices.push_back(normals[idx * 3 + 1]);
                data.vertices.push_back(normals[idx * 3 + 2]);
            } else {
                data.vertices.insert(data.vertices.end(), {0.0f, 0.0f, 0.0f});
            }
            if (normals && idx < normAccessor->count) {
                // safe to read normal
            } else {
                data.vertices.insert(data.vertices.end(), {0.0f, 0.0f, 0.0f});
            }

            // UV
            if (uvs) {
                data.vertices.push_back(uvs[idx * 2 + 0]);
                data.vertices.push_back(uvs[idx * 2 + 1]);
            } else {
                data.vertices.insert(data.vertices.end(), {0.0f, 0.0f});
            }


            if (uvs && idx < uvAccessor->count) {
                // safe to read uv
            } else {
                data.vertices.insert(data.vertices.end(), {0.0f, 0.0f});
            }


            // Sequential index into new vertex array
            data.indices.push_back(static_cast<unsigned int>(i));
        }

        data.indexType = GL_UNSIGNED_INT; // we generate sequential unsigned int indices

        std::cout << "Vertices: " << data.vertices.size() / 8   // pos(3)+norm(3)+uv(2) = 8 floats per vertex
                  << " Indices: " << data.indices.size()
                  << " Primitive indices accessor: " << primitive.indices << std::endl;
    }

    return data;
}



