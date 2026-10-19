#include "Engine.h"

#include "EngineConfig.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <utility>

void Engine::handleKeyboardInput(const CameraInput& input, const float deltaTime) {
    if (!playerExists) {
        if (input.forward) camera.moveForward(cameraSpeed, deltaTime);
        if (input.backward) camera.moveBackward(cameraSpeed, deltaTime);
        if (input.left) camera.moveLeft(cameraSpeed, deltaTime);
        if (input.right) camera.moveRight(cameraSpeed, deltaTime);
        if (input.up) camera.moveUp(cameraSpeed, deltaTime);
        if (input.down) camera.moveDown(cameraSpeed, deltaTime);
        return;
    }

    const float throttle = static_cast<float>(input.forward) -
                           static_cast<float>(input.backward);
    const float steering = static_cast<float>(input.right) -
                           static_cast<float>(input.left);
    player.drive(throttle, steering, deltaTime, collisions.getGroundNormal(player.body));
}

void Engine::handleMouseInput(const double xpos, const double ypos) {
    camera.mouseInput(xpos, ypos);
}

void Engine::togglePlayer() {
    if (playerExists) {
        collisions.removeRigidBody(player.body);
        player.body = nullptr;
        playerObject.body = nullptr;
        playerCarObjects.clear();
        playerExists = false;
        return;
    }

    const auto& carMeshes = loadModelMeshes("../assets/F1.glb");
    if (carMeshes.empty()) {
        std::cerr << "Cannot enter player mode: F1 car model has no renderable meshes" << std::endl;
        return;
    }

    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (const auto& mesh : carMeshes) {
        for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 8) {
            const glm::vec3 point(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
            minimum = glm::min(minimum, point);
            maximum = glm::max(maximum, point);
        }
    }
    const glm::vec3 modelCenter = (minimum + maximum) * 0.5f;

    glm::vec3 initialForward = camera.getFront();
    initialForward.y = 0.0f;
    const float initialYaw = glm::dot(initialForward, initialForward) > 0.0f
                                 ? std::atan2(initialForward.x, initialForward.z)
                                 : 0.0f;
    player.rotation = glm::angleAxis(initialYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    camera.setDirection(player.rotation * glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::vec3 carModelPosition =
        camera.getPosition() - player.rotation * engine_config::cockpitOffset;
    player.position = carModelPosition + player.rotation * engine_config::chassisOffset;
    playerObject.position = player.position;
    playerObject.rotation = player.rotation;
    playerObject.scale = glm::vec3(1.0f);
    MeshData collisionMesh;
    for (const auto& sourceMesh : carMeshes) {
        MeshData mesh = sourceMesh;
        for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 8) {
            mesh.vertices[i] -= modelCenter.x;
            mesh.vertices[i + 1] -= modelCenter.y;
            mesh.vertices[i + 2] -= modelCenter.z;
        }
        SceneObject carObject;
        carObject.buffers.init(mesh);
        carObject.position = player.position - player.rotation * engine_config::chassisOffset;
        carObject.rotation = player.rotation;
        carObject.textureID = mesh.textureID;
        carObject.baseColor = mesh.baseColor;
        carObject.transparent = mesh.transparent;
        carObject.alphaMasked = mesh.alphaMasked;
        carObject.alphaCutoff = mesh.alphaCutoff;
        playerCarObjects.push_back(std::move(carObject));
    }
    for (const float x : {-engine_config::chassisHalfExtents.x,
                          engine_config::chassisHalfExtents.x}) {
        for (const float y : {-engine_config::chassisHalfExtents.y,
                              engine_config::chassisHalfExtents.y}) {
            for (const float z : {-engine_config::chassisHalfExtents.z,
                                  engine_config::chassisHalfExtents.z}) {
                collisionMesh.vertices.insert(collisionMesh.vertices.end(),
                                              {x, y, z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f});
            }
        }
    }
    player.body = collisions.boxShapeD(playerObject, collisionMesh);
    if (player.body == nullptr) {
        std::cerr << "Failed to spawn player body" << std::endl;
        playerCarObjects.clear();
        return;
    }
    player.body->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
    player.body->setFriction(0.8f);
    player.body->setRestitution(0.0f);
    player.body->setDamping(0.05f, 0.85f);
    playerExists = true;
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
    if (playerExists) {
        update(playerObject);
        player.position = playerObject.position;
        player.rotation = playerObject.rotation;
        for (auto& carObject : playerCarObjects) {
            carObject.position = player.position - player.rotation * engine_config::chassisOffset;
            carObject.rotation = player.rotation;
        }
    }
}

void Player::drive(const float throttle, const float steering, const float deltaTime,
                   const glm::vec3& groundNormal) {
    if (body == nullptr) {
        return;
    }

    const bool grounded = groundNormal.y > 0.2f;
    glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
    const btVector3 velocity = body->getLinearVelocity();
    if (throttle != 0.0f) {
        if (grounded) {
            forward -= groundNormal * glm::dot(forward, groundNormal);
        } else {
            forward.y = 0.0f;
        }
        if (glm::dot(forward, forward) > 0.0001f) {
            forward = glm::normalize(forward);
        }
        const float driveSpeed = throttle > 0.0f ? speed : speed * 0.45f;
        const float acceleration = std::min(deltaTime * 6.0f, 1.0f);
        const float targetX = forward.x * driveSpeed * throttle;
        const float targetZ = forward.z * driveSpeed * throttle;
        body->setLinearVelocity(btVector3(
            velocity.x() + (targetX - velocity.x()) * acceleration,
            velocity.y(),
            velocity.z() + (targetZ - velocity.z()) * acceleration));
    } else {
        body->setLinearVelocity(btVector3(velocity.x() * 0.99f, velocity.y(),
                                          velocity.z() * 0.99f));
    }

    btVector3 angularVelocity = body->getAngularVelocity();
    angularVelocity.setX(angularVelocity.x() * std::exp(-2.0f * deltaTime));
    angularVelocity.setZ(angularVelocity.z() * std::exp(-2.0f * deltaTime));
    const btVector3 worldUp(0.0f, 1.0f, 0.0f);
    btVector3 targetUp = grounded
                             ? btVector3(groundNormal.x, groundNormal.y, groundNormal.z)
                             : worldUp;
    if (grounded) {
        targetUp.normalize();
    }
    const btVector3 carUp = body->getWorldTransform().getBasis() * btVector3(0.0f, 1.0f, 0.0f);
    const btScalar uprightness = carUp.dot(targetUp);
    if (uprightness < btScalar(0.8660254f)) {
        const btVector3 correctionTorque = carUp.cross(targetUp) * btScalar(35.0f);
        body->applyTorque(correctionTorque);
    }
    constexpr btScalar maxRollPitchRate = btScalar(2.5f);
    angularVelocity.setX(btClamped(angularVelocity.x(), -maxRollPitchRate, maxRollPitchRate));
    angularVelocity.setZ(btClamped(angularVelocity.z(), -maxRollPitchRate, maxRollPitchRate));
    const float horizontalSpeed = std::sqrt(velocity.x() * velocity.x() +
                                            velocity.z() * velocity.z());
    const float steeringDirection = throttle < 0.0f ? -1.0f : 1.0f;
    const float speedFactor = std::clamp(horizontalSpeed / std::max(speed, 1.0f), 0.0f, 1.0f);
    const float targetYawRate = -steering * 2.2f * speedFactor * steeringDirection;
    const float steeringResponse = std::min(deltaTime * 8.0f, 1.0f);
    angularVelocity.setY(angularVelocity.y() +
                         (targetYawRate - angularVelocity.y()) * steeringResponse);
    body->setAngularVelocity(angularVelocity);
    body->activate(true);
}
