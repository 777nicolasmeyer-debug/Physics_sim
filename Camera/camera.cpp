//
// Created by 777ni on 2026/09/13.
//

#include "camera.h"

Camera::Camera() {
    cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;

    calculateVectors();
}

void Camera::moveForward(const float speed, const float deltaTime) {
    cameraPos += speed * cameraFront * deltaTime;
    calculateVectors();
}
void Camera::moveBackward(const float speed, const float deltaTime) {
    cameraPos -= speed * cameraFront * deltaTime;
    calculateVectors();
}
void Camera::moveLeft(const float speed, const float deltaTime) {
    cameraPos -= speed * cameraRight * deltaTime;
    calculateVectors();
}
void Camera::moveRight(const float speed, const float deltaTime) {
    cameraPos += speed * cameraRight * deltaTime;
    calculateVectors();
}
void Camera::moveUp(const float speed, const float deltaTime) {
    cameraPos += speed * cameraUp * deltaTime;
    calculateVectors();
}
void Camera::moveDown(const float speed, const float deltaTime) {
    cameraPos -= speed * cameraUp * deltaTime;
    calculateVectors();
}

void Camera::mouseInput(double x, double y, const float deltaTime) {
    const float sensitivity = 100.0f;
    float Xoffset = static_cast<float>(x) - lastX;
    float Yoffset = static_cast<float>(y) - lastY;
    lastX = static_cast<float>(x);
    lastY = static_cast<float>(y);

    Xoffset *= sensitivity;
    Yoffset *= sensitivity;

    yaw += Xoffset * deltaTime;
    pitch -= Yoffset * deltaTime;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (firstMove) {
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
    }
    calculateVectors();
}

void Camera::calculateVectors() {
    cameraDirection.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    cameraDirection.y = std::sin(glm::radians(pitch));
    cameraDirection.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    cameraFront = glm::normalize(cameraDirection);
    cameraRight = glm::normalize(glm::cross(cameraFront, up));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

glm::vec3 Camera::getPosition() const {
    return cameraPos;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
