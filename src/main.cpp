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

#include "../Scene/SceneManager.h"


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Shader shader;
Loaders loader;
Matrix matrix(WIDTH, HEIGHT);
Camera camera;
Gravity gravity;
Collisions collisions;

tinygltf::Model Object1;
VAO_VBO objectBuffers;
VAO_VBO buffers;
std::vector<SceneObject> sceneObjects;

static float speed = 5.0f;
static float deltaTime = 0.0f;

static void spawnObject1();
static void reset();
static void handle_keyboardInput(GLFWwindow* window);
static void mouse(GLFWwindow* window, double xpos, double ypos) {
    camera.mouseInput(xpos, ypos, deltaTime);
}

unsigned int crateTex;
unsigned int planeTex;
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

    crateTex = loader.loadImageFromFile("../assets/crate.png");
    planeTex = loader.loadImageFromFile("../assets/plane.png");


    collisions.init();
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

        for (auto& obj : sceneObjects) {
            obj.draw(shader);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}

static bool SpawnCubeWasDown = false;
static bool SpawnObjectWasDown = false;
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

    if (glfwGetKey(window, GLFW_KEY_R)) {
        reset();
    }
    if (glfwGetKey(window, GLFW_KEY_2)) {
        if (!SpawnObjectWasDown) {
            spawnObject1();
            SpawnObjectWasDown = true;
        }
    }
    else {
        SpawnObjectWasDown = false;
    }
}

void spawnObject1() {
    MeshData object1;
    glBindTexture(GL_TEXTURE_2D, crateTex);
    if (loader.loadModelFromFile("../assets/Object1.glb", Object1)) {
        object1 = loader.extractMeshData(Object1, 0);
        SceneObject obj;
        obj.buffers.init(object1);
        obj.position = camera.getPosition();
        obj.textureID = crateTex;
        obj.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        obj.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        sceneObjects.push_back(obj);
    }

}

void reset() {
    sceneObjects.clear();
}