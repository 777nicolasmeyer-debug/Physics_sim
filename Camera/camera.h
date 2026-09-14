//
// Created by 777ni on 2026/09/13.
//

#ifndef PHYSICS_SIM_CAMERA_H
#define PHYSICS_SIM_CAMERA_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include <cmath>

class Camera {
public:
    Camera();
    glm::mat4 view();
    void moveForward(float speed, float deltaTime);
    void moveBackward(float , float deltaTime);
    void moveLeft(float speed, float deltaTime);
    void moveRight(float speed, float deltaTime);
    void moveUp(float speed, float deltaTime);
    void moveDown(float speed, float deltaTime);

    void calculateVectors();
    void mouseInput( double x, double y, float deltaTime);
    glm::vec3 getPosition() const;
    glm::mat4 getViewMatrix() const;
private:
    bool firstMove = true;
    float yaw;
    float pitch;
    float xpos = 400;
    float ypos = 400;
    float lastX, lastY;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraPos;

    glm::vec3 cameraDirection;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
};
#endif //PHYSICS_SIM_CAMERA_H
