//
// Created by 777ni on 2026/09/16.
//

#ifndef PHYSICS_SIM_SCENEMANAGER_H
#define PHYSICS_SIM_SCENEMANAGER_H


#include "../Rendering/VAO_VBO.h"
#include "../Shader/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SceneObject {
    VAO_VBO buffers;
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    GLuint textureID;

    void draw(Shader& shader);
};


#endif //PHYSICS_SIM_SCENEMANAGER_H
