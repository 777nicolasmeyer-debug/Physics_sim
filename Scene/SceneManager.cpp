//
// Created by 777ni on 2026/09/16.
//

#include "SceneManager.h"

void SceneObject::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));
    model = glm::scale(model, scale);

    shader.loadMatrix("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    buffers.draw();
}