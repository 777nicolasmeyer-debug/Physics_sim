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
#include <string>


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr unsigned int SHADOW_MAP_SIZE = 1024;
constexpr float SHADOW_FAR_PLANE = 200.0f;

Shader shader;
Shader shadowShader;

Matrix matrix(WIDTH, HEIGHT);
Camera camera;
Gravity gravity;
Collisions collisions;

std::vector<SceneObject> sceneObjects;
std::vector<SceneObject> terrainObjects;
std::vector<LightingObject> lights;

static float speed = 5.0f;
static float deltaTime = 0.0f;

static void spawnLight(Loaders& loader);
static void spawnObject1(Loaders& loader);
static void terainInit(Loaders& loader);
static void reset();
static void handle_keyboardInput(GLFWwindow* window, Loaders& loader);
static void mouse(GLFWwindow* window, double xpos, double ypos) {
    camera.mouseInput(xpos, ypos, deltaTime);
}

unsigned int crateTex;
unsigned int planeTex;
unsigned int terrainTex;

static GLuint shadowFramebuffer = 0;

static glm::mat4 makeModelMatrix(const SceneObject& obj) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, obj.position);
    model *= glm::mat4_cast(obj.rotation);
    return glm::scale(model, obj.scale);
}

static void updatePhysicsTransforms() {
    const auto update = [](SceneObject& obj) {
        btTransform transform;
        obj.body->getMotionState()->getWorldTransform(transform);
        obj.position = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());
        const btQuaternion rotation = transform.getRotation();
        obj.rotation = glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    };
    for (auto& obj : sceneObjects) update(obj);
    for (auto& obj : terrainObjects) update(obj);
}

static bool initializeShadowMap() {
    shadowShader.createShaders("../Shader/shadow_vertex.glsl",
                               "../Shader/shadow_geometry.glsl",
                               "../Shader/shadow_fragment.glsl");
    if (shadowShader.ID == 0) return false;

    glGenFramebuffers(1, &shadowFramebuffer);
    return shadowFramebuffer != 0;
}

static GLuint createShadowCubemap() {
    GLuint shadowCubemap = 0;
    glGenTextures(1, &shadowCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemap);
    for (unsigned int face = 0; face < 6; ++face) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                     SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (!complete) {
        std::cerr << "Failed to create point-light shadow cubemap" << std::endl;
        glDeleteTextures(1, &shadowCubemap);
        return 0;
    }
    return shadowCubemap;
}

static void renderShadowMap(GLuint shadowCubemap, const glm::vec3& lightPosition) {
    constexpr glm::vec3 directions[] = {
        { 1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},
        { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f, -1.0f}
    };
    constexpr glm::vec3 ups[] = {
        {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f},
        {0.0f,  0.0f,  1.0f}, {0.0f,  0.0f, -1.0f},
        {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f}
    };
    const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, SHADOW_FAR_PLANE);
    glm::mat4 shadowMatrices[6];
    for (int face = 0; face < 6; ++face) {
        shadowMatrices[face] = projection * glm::lookAt(lightPosition, lightPosition + directions[face], ups[face]);
    }

    shadowShader.use();
    glUniformMatrix4fv(glGetUniformLocation(shadowShader.ID, "shadowMatrices[0]"), 6, GL_FALSE,
                       glm::value_ptr(shadowMatrices[0]));
    shadowShader.loadVector3("lightPos", lightPosition);
    glUniform1f(glGetUniformLocation(shadowShader.ID, "farPlane"), SHADOW_FAR_PLANE);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubemap, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& obj : terrainObjects) {
        shadowShader.loadMatrix("model", makeModelMatrix(obj));
        obj.buffers.draw();
    }
    for (const auto& obj : sceneObjects) {
        shadowShader.loadMatrix("model", makeModelMatrix(obj));
        obj.buffers.draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WIDTH, HEIGHT);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    Loaders loader;

    crateTex = loader.loadImageFromFile("../assets/crate.png");
    planeTex = loader.loadImageFromFile("../assets/plane.png");
    terrainTex = loader.loadImageFromFile("../assets/Wood_Tower_Col.png");


    collisions.init();
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);
    const bool shadowMapReady = initializeShadowMap();
    if (!shadowMapReady) {
        std::cerr << "Point-light shadows are disabled" << std::endl;
    }
    shader.loadTexture("shadowCube", 1);

    terainInit(loader);
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    auto lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        collisions.update(deltaTime);
        updatePhysicsTransforms();
        handle_keyboardInput(window, loader);
        if (shadowMapReady) {
            for (const auto& light : lights) {
                if (light.shadowCubemap != 0) {
                    renderShadowMap(light.shadowCubemap, light.position);
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.loadMatrix("projection", matrix.projection);
        shader.loadMatrix("view", camera.getViewMatrix());
        shader.loadVector3("viewPos", camera.getPosition());
        glUniform1f(glGetUniformLocation(shader.ID, "farPlane"), SHADOW_FAR_PLANE);
        if (lights.empty()) {
            shader.loadVector3("lightColor", glm::vec3(0.0f));
            shader.loadVector3("lightPos", glm::vec3(0.0f));
            glUniform1i(glGetUniformLocation(shader.ID, "shadowsEnabled"), 0);
            for (auto& obj : sceneObjects) obj.draw(shader);
            for (auto& obj : terrainObjects) obj.draw(shader);
        } else {
            for (size_t i = 0; i < lights.size(); ++i) {
                const auto& light = lights[i];
                shader.loadVector3("lightColor", light.color);
                shader.loadVector3("lightPos", light.position);
                glUniform1i(glGetUniformLocation(shader.ID, "shadowsEnabled"),
                            light.shadowCubemap != 0 ? 1 : 0);
                glUniform1i(glGetUniformLocation(shader.ID, "ambientEnabled"), i == 0 ? 1 : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadowCubemap);
                if (i == 0) {
                    glDisable(GL_BLEND);
                } else {
                    glEnable(GL_BLEND);
                    glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
                }
                glDepthFunc(i == 0 ? GL_LESS : GL_LEQUAL);
                glDepthMask(i == 0 ? GL_TRUE : GL_FALSE);

                if (i == 0) {
                    for (auto& marker : lights) marker.draw(shader);
                }
                for (auto& obj : sceneObjects) obj.draw(shader);
                for (auto& obj : terrainObjects) obj.draw(shader);
            }
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);

    }
    glfwTerminate();
}

static bool SpawnLightWasDown = false;
static bool SpawnObjectWasDown = false;
void handle_keyboardInput(GLFWwindow* window, Loaders& loader) {
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
    if (glfwGetKey(window, GLFW_KEY_L)) {
        if (!SpawnLightWasDown) {
            spawnLight(loader);
            SpawnLightWasDown = true;
        }
    }
    else {
        SpawnLightWasDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_2)) {
        if (!SpawnObjectWasDown) {
            spawnObject1(loader);
            SpawnObjectWasDown = true;
        }
    }
    else {
        SpawnObjectWasDown = false;
    }
}

void terainInit(Loaders& loader) {
    tinygltf::Model model;
    loader.loadModelFromFile("../assets/WatchTower.glb", model);

    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        MeshData terrainData = loader.extractMeshData(model, i);

        SceneObject obj;
        obj.buffers.init(terrainData);
        obj.position = glm::vec3(0.0f);
        obj.rotation = glm::angleAxis(glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
        obj.scale = glm::vec3(1.0f);
        obj.textureID = terrainData.textureID;
        obj.baseColor = terrainData.baseColor;
        std::cout << "TextureID: " << obj.textureID << std::endl;
        collisions.triangleShapeS(obj, terrainData);

        terrainObjects.push_back(obj);
    }
}

void spawnLight(Loaders& loader) {
    tinygltf::Model model;
    MeshData lightData;
    loader.loadModelFromFile("../assets/Light.glb", model);
    lightData = loader.extractMeshData(model, 0);

    LightingObject obj;
    obj.buffers.init(lightData);
    obj.position = camera.getPosition();
    if (shadowFramebuffer != 0) {
        obj.shadowCubemap = createShadowCubemap();
    }
    lights.push_back(obj);
}

void spawnObject1(Loaders& loader) {
    tinygltf::Model Object;
    MeshData objectData;
    loader.loadModelFromFile("../assets/Object1.glb", Object);
    objectData = loader.extractMeshData(Object, 0);

    SceneObject obj;
    obj.buffers.init(objectData);
    obj.position = camera.getPosition();
    obj.textureID = objectData.textureID;
    obj.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    obj.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    obj.baseColor = objectData.baseColor;
    collisions.convexShapeD(obj, objectData);
    sceneObjects.push_back(obj);


    Object = tinygltf::Model();
    objectData.vertices.clear();
    objectData.indices.clear();
}



void reset() {
    sceneObjects.clear();
    for (auto& light : lights) {
        if (light.shadowCubemap != 0) {
            glDeleteTextures(1, &light.shadowCubemap);
        }
    }
    lights.clear();
}