//
// Created by 777ni on 2026/09/16.
//

#include "SceneManager.h"

#include <glm/gtc/quaternion.hpp>

void SceneObject::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(rotation);
    model = glm::scale(model, scale);

    shader.loadMatrix("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    buffers.draw();
}
