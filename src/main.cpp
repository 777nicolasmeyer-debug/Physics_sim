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
#include <vector>


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Shader shader;
Meshes::Cube cube;
VAO_VBO buffers;
Loaders loader;
Matrix matrix(WIDTH, HEIGHT);
Camera camera;
Gravity gravity;

std::vector<Meshes::Cube> cubes;

static float speed = 5.0f;
float deltaTime = 0.0f;

static void spawnCube();
static void reset();
static void handle_keyboardInput(GLFWwindow* window, float deltaTime);
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

    buffers.setCubeVBO(cube);
    buffers.setCubeVAO(cube);

    unsigned int crateTex = loader.loadImageFromFile("../assets/crate.png");
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);


    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    auto lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handle_keyboardInput(window, deltaTime);

        shader.use();
        shader.loadMatrix("projection", matrix.projection);
        shader.loadMatrix("view", camera.getViewMatrix());
        for (Meshes::Cube& cube1 : cubes) {
            gravity.apply(cube1.velocity, deltaTime);
            cube1.position += cube1.velocity * deltaTime;
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cube1.position);
            shader.loadMatrix("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, crateTex);
            glBindVertexArray(buffers.vao);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(cube.vertices.size() / 5));
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}

void handle_keyboardInput(GLFWwindow* window, float deltaTime) {
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
        spawnCube();
    }
    else if (glfwGetKey(window, GLFW_KEY_R)) {
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