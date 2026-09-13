//
// Created by 777ni on 2026/09/11.
//

#ifndef PHYSICS_SIM_LOADERS_H
#define PHYSICS_SIM_LOADERS_H
#include "stb_image.h"
#include <glad/glad.h>

class Loaders {
public:
    unsigned int loadImageFromFile(const char* path);
    unsigned int texture;
private:
    unsigned char* data;
};

#endif //PHYSICS_SIM_LOADERS_H
