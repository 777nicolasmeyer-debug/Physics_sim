#include "../Engine/Engine.h"

#include <iostream>
#include <functional>
#include <limits>
#include <unordered_set>
#include <utility>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

glm::mat4 makeModelMatrix(const SceneObject& object) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, object.position);
    model *= glm::mat4_cast(object.rotation);
    return glm::scale(model, object.scale);
}

glm::mat4 getNodeTransform(const tinygltf::Node& node) {
    if (node.matrix.size() == 16) {
        glm::mat4 transform(1.0f);
        for (size_t column = 0; column < 4; ++column) {
            for (size_t row = 0; row < 4; ++row) {
                transform[column][row] = static_cast<float>(node.matrix[column * 4 + row]);
            }
        }
        return transform;
    }

    glm::mat4 transform(1.0f);
    if (node.translation.size() == 3) {
        transform = glm::translate(transform, glm::vec3(
            static_cast<float>(node.translation[0]),
            static_cast<float>(node.translation[1]),
            static_cast<float>(node.translation[2])));
    }
    if (node.rotation.size() == 4) {
        transform *= glm::mat4_cast(glm::quat(
            static_cast<float>(node.rotation[3]),
            static_cast<float>(node.rotation[0]),
            static_cast<float>(node.rotation[1]),
            static_cast<float>(node.rotation[2])));
    }
    if (node.scale.size() == 3) {
        transform = glm::scale(transform, glm::vec3(
            static_cast<float>(node.scale[0]),
            static_cast<float>(node.scale[1]),
            static_cast<float>(node.scale[2])));
    }
    return transform;
}

void applyNodeTransform(MeshData& mesh, const glm::mat4& transform) {
    const glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(transform)));
    for (size_t i = 0; i + 7 < mesh.vertices.size(); i += 8) {
        const glm::vec3 position = glm::vec3(transform * glm::vec4(
            mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2], 1.0f));
        mesh.vertices[i] = position.x;
        mesh.vertices[i + 1] = position.y;
        mesh.vertices[i + 2] = position.z;

        const glm::vec3 normal = normalTransform *
            glm::vec3(mesh.vertices[i + 3], mesh.vertices[i + 4], mesh.vertices[i + 5]);
        if (glm::dot(normal, normal) > 0.0f) {
            const glm::vec3 normalized = glm::normalize(normal);
            mesh.vertices[i + 3] = normalized.x;
            mesh.vertices[i + 4] = normalized.y;
            mesh.vertices[i + 5] = normalized.z;
        }
    }
}

}

Engine::~Engine() {
    if (!initialized) {
        return;
    }
    clearDynamicObjects();
    for (const auto& light : lights) {
        if (light.shadowCubemap != 0) {
            glDeleteTextures(1, &light.shadowCubemap);
        }
    }
    if (shadowFramebuffer != 0) {
        glDeleteFramebuffers(1, &shadowFramebuffer);
    }
}

bool Engine::initialize() {
    if (initialized) {
        return true;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    loader.initialize();
    collisions.init();
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();
    shader.loadTexture("tex", 0);
    shadowMapReady = initializeShadowMap();
    if (!shadowMapReady) {
        std::cerr << "Point-light shadows are disabled" << std::endl;
    }
    shader.loadTexture("shadowCube", 1);
    initialized = true;
    return true;
}

void Engine::update(float deltaTime) {
    if (!initialized) {
        return;
    }
    collisions.update(deltaTime);
    updatePhysicsTransforms();
}

void Engine::render(const int width, const int height) {
    if (!initialized || width <= 0 || height <= 0) {
        return;
    }

    glViewport(0, 0, width, height);
    if (shadowMapReady) {
        for (const auto& light : lights) {
            if (light.shadowCubemap != 0) {
                renderShadowMap(light.shadowCubemap, light.position, width, height);
            }
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 200.0f);
    shader.loadMatrix("projection", projection);
    shader.loadMatrix("view", camera.getViewMatrix());
    shader.loadVector3("viewPos", camera.getPosition());
    glUniform1f(glGetUniformLocation(shader.ID, "farPlane"), SHADOW_FAR_PLANE);

    if (lights.empty()) {
        shader.loadVector3("lightColor", glm::vec3(0.0f));
        shader.loadVector3("lightPos", glm::vec3(0.0f));
        glUniform1i(glGetUniformLocation(shader.ID, "shadowsEnabled"), 0);
        for (auto& object : sceneObjects) object.draw(shader);
        for (auto& object : terrainObjects) object.draw(shader);
        return;
    }

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
        for (auto& object : sceneObjects) object.draw(shader);
        for (auto& object : terrainObjects) object.draw(shader);
    }
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
}

void Engine::handleKeyboardInput(const CameraInput& input, const float deltaTime) {
    if (input.forward) camera.moveForward(cameraSpeed, deltaTime);
    if (input.backward) camera.moveBackward(cameraSpeed, deltaTime);
    if (input.left) camera.moveLeft(cameraSpeed, deltaTime);
    if (input.right) camera.moveRight(cameraSpeed, deltaTime);
    if (input.up) camera.moveUp(cameraSpeed, deltaTime);
    if (input.down) camera.moveDown(cameraSpeed, deltaTime);
}

void Engine::handleMouseInput(const double xpos, const double ypos) {
    camera.mouseInput(xpos, ypos);
}

void Engine::addStaticObject(const MeshData& mesh, const glm::vec3& position,
                             const glm::quat& rotation, const glm::vec3& scale) {
    SceneObject object;
    object.buffers.init(mesh);
    object.position = position;
    object.rotation = rotation;
    object.scale = scale;
    object.textureID = mesh.textureID;
    object.baseColor = mesh.baseColor;
    collisions.triangleShapeS(object, mesh);
    terrainObjects.push_back(object);
}

void Engine::addStaticModel(const char* path, const glm::vec3& position,
                            const glm::quat& rotation, const glm::vec3& scale) {
    for (const auto& mesh : loadModelMeshes(path)) {
        addStaticObject(mesh, position, rotation, scale);
    }
}

void Engine::addDynamicObject(const MeshData& mesh, const glm::vec3& position,
                              const glm::quat& rotation, const glm::vec3& scale) {
    if (mesh.vertices.empty()) {
        return;
    }

    SceneObject object;
    object.buffers.init(mesh);
    object.position = position;
    object.rotation = rotation;
    object.scale = scale;
    object.textureID = mesh.textureID;
    object.baseColor = mesh.baseColor;
    collisions.convexShapeD(object, mesh, scale);
    sceneObjects.push_back(object);
}

void Engine::addDynamicModel(const std::vector<MeshData>& meshes, const glm::vec3& position,
                             const glm::quat& rotation, const glm::vec3& scale) {
    if (meshes.empty()) {
        return;
    }

    if (meshes.size() == 1) {
        addDynamicObject(meshes.front(), position, rotation, scale);
        return;
    }

    MeshData collisionMesh;
    size_t totalVertexCount = 0;
    for (const auto& mesh : meshes) {
        totalVertexCount += mesh.vertices.size();
    }
    collisionMesh.vertices.reserve(totalVertexCount);

    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (const auto& mesh : meshes) {
        for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 8) {
            const glm::vec3 point(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
            minimum = glm::min(minimum, point);
            maximum = glm::max(maximum, point);
        }
    }
    const glm::vec3 modelCenter = (minimum + maximum) * 0.5f;

    for (const auto& mesh : meshes) {
        collisionMesh.vertices.insert(collisionMesh.vertices.end(),
                                      mesh.vertices.begin(), mesh.vertices.end());
    }
    for (size_t i = 0; i + 2 < collisionMesh.vertices.size(); i += 8) {
        collisionMesh.vertices[i] -= modelCenter.x;
        collisionMesh.vertices[i + 1] -= modelCenter.y;
        collisionMesh.vertices[i + 2] -= modelCenter.z;
    }

    SceneObject physicsObject;
    physicsObject.position = position;
    physicsObject.rotation = rotation;
    collisions.boxShapeD(physicsObject, collisionMesh, scale);

    for (const auto& mesh : meshes) {
        if (mesh.vertices.empty()) {
            continue;
        }
        MeshData centeredMesh = mesh;
        for (size_t i = 0; i + 2 < centeredMesh.vertices.size(); i += 8) {
            centeredMesh.vertices[i] -= modelCenter.x;
            centeredMesh.vertices[i + 1] -= modelCenter.y;
            centeredMesh.vertices[i + 2] -= modelCenter.z;
        }
        SceneObject object;
        object.buffers.init(centeredMesh);
        object.position = position;
        object.rotation = rotation;
        object.scale = scale;
        object.textureID = centeredMesh.textureID;
        object.baseColor = centeredMesh.baseColor;
        object.body = physicsObject.body;
        sceneObjects.push_back(object);
    }
}

void Engine::addDynamicModel(const char* path, const glm::vec3& position,
                             const glm::quat& rotation, const glm::vec3& scale) {
    addDynamicModel(loadModelMeshes(path), position, rotation, scale);
}

void Engine::addLight(const MeshData& markerMesh, const glm::vec3& position,
                      const glm::vec3& color) {
    LightingObject light;
    light.buffers.emplace_back();
    light.buffers.back().init(markerMesh);
    light.position = position;
    light.color = color;
    if (shadowFramebuffer != 0) {
        light.shadowCubemap = createShadowCubemap();
    }
    lights.push_back(light);
}

void Engine::addLight(const char* modelPath, const glm::vec3& position,
                      const glm::vec3& color) {
    LightingObject light;
    for (const auto& mesh : loadModelMeshes(modelPath)) {
        if (mesh.vertices.empty()) {
            continue;
        }
        light.buffers.emplace_back();
        light.buffers.back().init(mesh);
    }
    if (light.buffers.empty()) {
        std::cerr << "Light model contains no renderable meshes: " << modelPath << std::endl;
        return;
    }
    light.position = position;
    light.color = color;
    if (shadowFramebuffer != 0) {
        light.shadowCubemap = createShadowCubemap();
    }
    lights.push_back(light);
}

void Engine::clearDynamicObjects() {
    std::unordered_set<btRigidBody*> bodies;
    for (auto& object : sceneObjects) {
        if (object.body != nullptr && bodies.insert(object.body).second) {
            collisions.removeRigidBody(object.body);
        }
        object.body = nullptr;
    }
    sceneObjects.clear();
}

void Engine::clearLights() {
    for (auto& light : lights) {
        if (light.shadowCubemap != 0) {
            glDeleteTextures(1, &light.shadowCubemap);
            light.shadowCubemap = 0;
        }
    }
    lights.clear();
}

glm::vec3 Engine::getCameraPosition() const {
    return camera.getPosition();
}

glm::vec3 Engine::getCameraForward() const {
    return camera.getFront();
}

const std::vector<MeshData>& Engine::loadModelMeshes(const char* path) {
    const auto [entry, inserted] = modelCache.try_emplace(path);
    if (!inserted) {
        return entry->second;
    }

    tinygltf::Model model;
    loader.loadModelFromFile(path, model);
    if (model.meshes.empty()) {
        std::cerr << "Model contains no meshes: " << path << std::endl;
        return entry->second;
    }

    std::vector<bool> hasParent(model.nodes.size(), false);
    for (const auto& node : model.nodes) {
        for (const int child : node.children) {
            if (child >= 0 && static_cast<size_t>(child) < hasParent.size()) {
                hasParent[child] = true;
            }
        }
    }

    std::function<void(int, const glm::mat4&)> visitNode;
    visitNode = [&](const int nodeIndex, const glm::mat4& parentTransform) {
        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= model.nodes.size()) {
            return;
        }
        const auto& node = model.nodes[nodeIndex];
        const glm::mat4 worldTransform = parentTransform * getNodeTransform(node);
        if (node.mesh >= 0 && static_cast<size_t>(node.mesh) < model.meshes.size()) {
            MeshData mesh = loader.extractMeshData(model, node.mesh);
            applyNodeTransform(mesh, worldTransform);
            if (!mesh.vertices.empty()) {
                entry->second.push_back(std::move(mesh));
            }
        }
        for (const int child : node.children) {
            visitNode(child, worldTransform);
        }
    };

    if (!model.scenes.empty()) {
        const int sceneIndex = model.defaultScene >= 0 &&
                               static_cast<size_t>(model.defaultScene) < model.scenes.size()
                                   ? model.defaultScene
                                   : 0;
        for (const int rootNode : model.scenes[sceneIndex].nodes) {
            visitNode(rootNode, glm::mat4(1.0f));
        }
    } else {
        for (size_t i = 0; i < model.nodes.size(); ++i) {
            if (!hasParent[i]) {
                visitNode(static_cast<int>(i), glm::mat4(1.0f));
            }
        }
    }

    if (entry->second.empty()) {
        std::cerr << "Model scene contains no renderable meshes: " << path << std::endl;
    }
    return entry->second;
}

bool Engine::initializeShadowMap() {
    shadowShader.createShaders("../Shader/shadow_vertex.glsl",
                               "../Shader/shadow_geometry.glsl",
                               "../Shader/shadow_fragment.glsl");
    if (shadowShader.ID == 0) return false;

    glGenFramebuffers(1, &shadowFramebuffer);
    return shadowFramebuffer != 0;
}

GLuint Engine::createShadowCubemap() {
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

void Engine::renderShadowMap(const GLuint shadowCubemap, const glm::vec3& lightPosition,
                             const int width, const int height) {
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
    const glm::mat4 projection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, SHADOW_FAR_PLANE);
    glm::mat4 shadowMatrices[6];
    for (int face = 0; face < 6; ++face) {
        shadowMatrices[face] =
            projection * glm::lookAt(lightPosition, lightPosition + directions[face], ups[face]);
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

    for (const auto& object : terrainObjects) {
        shadowShader.loadMatrix("model", makeModelMatrix(object));
        object.buffers.draw();
    }
    for (const auto& object : sceneObjects) {
        shadowShader.loadMatrix("model", makeModelMatrix(object));
        object.buffers.draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

void Engine::updatePhysicsTransforms() {
    const auto update = [](SceneObject& object) {
        if (object.body == nullptr || object.body->getMotionState() == nullptr) {
            return;
        }
        btTransform transform;
        object.body->getMotionState()->getWorldTransform(transform);
        object.position = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(),
                                    transform.getOrigin().z());
        const btQuaternion rotation = transform.getRotation();
        object.rotation = glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    };
    for (auto& object : sceneObjects) update(object);
    for (auto& object : terrainObjects) update(object);
}
