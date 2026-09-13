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

void Camera::moveForward(const float speed) {
    cameraPos += speed * cameraFront;
    calculateVectors();
}
void Camera::moveBackward(const float speed) {
    cameraPos -= speed * cameraFront;
    calculateVectors();
}
void Camera::moveLeft(const float speed) {
    cameraPos -= speed * cameraRight;
    calculateVectors();
}
void Camera::moveRight(const float speed) {
    cameraPos += speed * cameraRight;
    calculateVectors();
}
void Camera::moveUp(const float speed) {
    cameraPos += speed * cameraUp;
    calculateVectors();
}
void Camera::moveDown(const float speed) {
    cameraPos -= speed * cameraUp;
    calculateVectors();
}

void Camera::mouseInput(double x, double y) {
    const float sensitivity = 0.1f;
    float Xoffset = static_cast<float>(x) - lastX;
    float Yoffset = static_cast<float>(y) - lastY;
    lastX = static_cast<float>(x);
    lastY = static_cast<float>(y);

    Xoffset *= sensitivity;
    Yoffset *= sensitivity;

    yaw += Xoffset;
    pitch += Yoffset;

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
    cameraRight = glm::normalize(glm::cross(cameraFront, cameraDirection));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
}

glm::vec3 Camera::getPosition() const {
    return cameraPos;
}
