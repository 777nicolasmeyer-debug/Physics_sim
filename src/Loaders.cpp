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
