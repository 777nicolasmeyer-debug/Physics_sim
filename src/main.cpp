#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Meshes/Meshes.h"
#include "../Shader/Shader.h"
#include "../Camera/camera.h"
#include "../Rendering/VAO_VBO.h"
#include "../Loaders/Loaders.h"
#include "../Math/matrix.h"
#include "../Math/gravity.h"
#include "../Math/collisions.h"
#include <vector>


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Shader shader;
Meshes::Cube cube;
Meshes::Plane plane;
VAO_VBO buffers;
Loaders loader;
Matrix matrix(WIDTH, HEIGHT);
Camera camera;
Gravity gravity;
Collisions collisions;


std::vector<Meshes::Cube> cubes;

static float speed = 5.0f;
static float deltaTime = 0.0f;

static void spawnCube();
static void reset();
static void handle_keyboardInput(GLFWwindow* window);
static void mouse(GLFWwindow* window, double xpos, double ypos) {
    camera.mouseInput(xpos, ypos, deltaTime);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Physics sim", nullptr, nullptr);
    if ( !window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse);


    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    buffers.createVBO();
    buffers.createVAO();

    unsigned int crateTex = loader.loadImageFromFile("../assets/crate.png");
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);


    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    auto lastFrame = static_cast<float>(glfwGetTime());
    plane.position.y = -20.f;
    plane.collider.center = plane.position;
    plane.collider.type = ColliderType::Plane;
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handle_keyboardInput(window);

        shader.use();
        shader.loadMatrix("projection", matrix.projection);
        shader.loadMatrix("view", camera.getViewMatrix());

        //cube
        for (Meshes::Cube& cube1 : cubes) {
            gravity.apply(cube1.velocity, deltaTime);
            cube1.position += cube1.velocity * deltaTime;
            cube1.collider.center = cube1.position;
            cube1.collider.type = ColliderType::Box;

            glm::mat4 rot(1.0f);
            rot = glm::rotate(rot, glm::radians(cube1.rotation.x), glm::vec3(1,0,0));
            rot = glm::rotate(rot, glm::radians(cube1.rotation.y), glm::vec3(0,1,0));
            rot = glm::rotate( rot, glm::radians(cube1.rotation.z), glm::vec3(0,0,1));

            cube1.collider.right = glm::normalize(glm::vec3(rot[0]));
            cube1.collider.up = glm::normalize(glm::vec3(rot[1]));
            cube1.collider.forward = glm::normalize(glm::vec3(rot[2]));


            bool isColliding = collisions.BoxBoxCollision(cube1.collider, plane.collider);
            if ( isColliding && ! cube1.wasColliding) {
                cube1.velocity.y *= -0.5f;
                float impactSpeed = std::abs(cube1.velocity.y);
                cube1.angularVel += glm::vec3(impactSpeed * 5.0f, impactSpeed * 2.0f, impactSpeed * 3.0f);
            }
            cube1.rotation += cube1.angularVel * deltaTime;
            cube1.wasColliding = isColliding;
            if (cube1.rotation.y <= 2.0f && glm::length(cube1.angularVel) <= 1.0f) {
                cube1.angularVel = glm::vec3(0.0f);
            }
            else {
                if (glm::length(cube1.angularVel) >= 1.0f) {
                    cube1.angularVel *= std::pow(0.99, deltaTime * 60.0f);
                }
            }

            for (int i = 0; i < cubes.size(); i++) {
                collisions.BoxBoxCollision(cube1.collider, cubes[i].collider);
            }

            cube1.position = cube1.collider.center;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cube1.position);
            model = glm::rotate(model, glm::radians(cube1.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(cube1.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(cube1.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            shader.loadMatrix("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, crateTex);
            glBindVertexArray(buffers.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //plane
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, plane.position);
        model = glm::scale(model,glm::vec3(10.0f, 1.0f, 10.0f));
        shader.loadMatrix("model", model);
        glBindTexture(GL_TEXTURE_2D, crateTex);
        glBindVertexArray(buffers.vao);
        glDrawArrays(GL_TRIANGLES, 36, 36);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}

static bool SpawnCubeWasDown = false;
void handle_keyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W)) {
        camera.moveForward(speed, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_S)) {
        camera.moveBackward(speed, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera.moveLeft(speed, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_D)) {
        camera.moveRight(speed, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        camera.moveUp(speed, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        camera.moveDown(speed, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_1)) {
        if (!SpawnCubeWasDown) {
            spawnCube();
            SpawnCubeWasDown = true;
        }
    }
    else {
        SpawnCubeWasDown = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R)) {
        reset();
    }
}

void spawnCube() {
    Meshes::Cube cube1;
    cube1.position = camera.getPosition();
    cube1.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    cubes.push_back(cube1);
}

void reset() {
    cubes.clear();
}