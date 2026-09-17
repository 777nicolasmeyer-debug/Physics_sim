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

std::vector<SceneObject> sceneObjects;
std::vector<SceneObject> terrainObjects;

static float speed = 5.0f;
static float deltaTime = 0.0f;

static void spawnObject1();
static void terainInit();
static void reset();
static void handle_keyboardInput(GLFWwindow* window);
static void mouse(GLFWwindow* window, double xpos, double ypos) {
    camera.mouseInput(xpos, ypos, deltaTime);
}

unsigned int crateTex;
unsigned int planeTex;
unsigned int terrainTex;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    crateTex = loader.loadImageFromFile("../assets/crate.png");
    planeTex = loader.loadImageFromFile("../assets/plane.png");
    terrainTex = loader.loadImageFromFile("../assets/Terrain.png");


    collisions.init();
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);

    terainInit();
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
            btTransform transform;

            obj.body -> getMotionState()->getWorldTransform(transform);
            obj.position = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());

            glm::quat orientation(transform.getRotation().w(), transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z());
            obj.rotation = orientation;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model *= glm::mat4_cast(obj.rotation);
            model = glm::scale(model, obj.scale);

            shader.loadMatrix("model", model);
            obj.draw(shader);

            shader.loadMatrix("model", model);
            obj.draw(shader);
        }
        for (auto& obj : terrainObjects) {
            btTransform transform;

            obj.body -> getMotionState()->getWorldTransform(transform);
            obj.position = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());

            glm::quat orientation(transform.getRotation().w(), transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z());
            obj.rotation = orientation;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model *= glm::mat4_cast(obj.rotation);
            model = glm::scale(model, obj.scale);

            shader.loadMatrix("model", model);
            obj.draw(shader);

            shader.loadMatrix("model", model);
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
    /*if (glfwGetKey(window, GLFW_KEY_1)) {
        if (!SpawnCubeWasDown) {
            spawnCube();
            SpawnCubeWasDown = true;
        }
    }
    else {
        SpawnCubeWasDown = false;
    }*/
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

void terainInit() {
    tinygltf::Model model;
    MeshData terrainData;
    loader.loadModelFromFile("../assets/Terrain.glb", model);
    terrainData = loader.extractMeshData(model, 0);

    SceneObject obj;
    obj.buffers.init(terrainData);
    obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
    obj.rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    obj.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    obj.textureID = terrainTex;
    collisions.convexShapeS(obj, terrainData);
    terrainObjects.push_back(obj);
}

void spawnObject1() {
    tinygltf::Model Object;
    MeshData objectData;
    loader.loadModelFromFile("../assets/Object1.glb", Object);
    objectData = loader.extractMeshData(Object, 0);

    SceneObject obj;
    obj.buffers.init(objectData);
    obj.position = camera.getPosition();
    obj.textureID = crateTex;
    obj.rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    obj.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    collisions.convexShapeD(obj, objectData);
    sceneObjects.push_back(obj);


    Object = tinygltf::Model();
    objectData.vertices.clear();
    objectData.indices.clear();
}



void reset() {
    sceneObjects.clear();
}