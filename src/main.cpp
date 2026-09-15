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
#include "../Math/Collisions.h"
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

    collisions.init();

    buffers.createVBO();
    buffers.createVAO();

    unsigned int crateTex = loader.loadImageFromFile("../assets/crate.png");
    unsigned int planeTex = loader.loadImageFromFile("../assets/plane.png");
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);


    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    auto lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        collisions.update(deltaTime);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handle_keyboardInput(window);

        shader.use();
        shader.loadMatrix("projection", matrix.projection);
        shader.loadMatrix("view", camera.getViewMatrix());

        //cube
       for (Meshes::Cube& cube1 : cubes) {
            btTransform transform;

            cube1.body -> getMotionState()->getWorldTransform(transform);
            cube1.position = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());

            btQuaternion rotation = transform.getRotation();
            glm::quat orientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
            glm::mat4 rotation1 = glm::mat4_cast(orientation);

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cube1.position);
            model *= rotation1;
            shader.loadMatrix("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, crateTex);
            glBindVertexArray(buffers.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //plane
        plane.position.y = -20.f;
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, plane.position);
        model = glm::scale(model,glm::vec3(100.0f, 1.0f, 100.0f));
        shader.loadMatrix("model", model);
        glBindTexture(GL_TEXTURE_2D, planeTex);
        glBindVertexArray(buffers.vao);
        glDrawArrays(GL_TRIANGLES, 72, 6);

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
    cube1.body = collisions.spawnCube(cube1.position);
    cubes.push_back(cube1);
}

void reset() {
    cubes.clear();
}