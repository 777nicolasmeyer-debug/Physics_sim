//
// Created by 777ni on 2026/09/15.
//

#include "Collisions.h"
#include <algorithm>
#include <bullet/btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <BulletCollision/CollisionDispatch/btManifoldResult.h>
#include <limits>
#include <vector>

static btBroadphaseInterface* broadphase;
static btDefaultCollisionConfiguration* collisionConfiguration;
static btCollisionDispatcher* dispatcher;
static btSequentialImpulseConstraintSolver* solver;
static btDiscreteDynamicsWorld* dynamicsWorld;

namespace {

bool adjustTrackEdgeContacts(btManifoldPoint& contact,
                             const btCollisionObjectWrapper* object0, const int partId0,
                             const int triangleIndex0,
                             const btCollisionObjectWrapper* object1, const int partId1,
                             const int triangleIndex1) {
    if ((object0->getCollisionObject()->getCollisionFlags() &
         btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) != 0) {
        btAdjustInternalEdgeContacts(contact, object0, object1, partId0, triangleIndex0);
    }
    if ((object1->getCollisionObject()->getCollisionFlags() &
         btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) != 0) {
        btAdjustInternalEdgeContacts(contact, object1, object0, partId1, triangleIndex1);
    }
    return true;
}

}

void Collisions::init() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
    dynamicsWorld->getSolverInfo().m_numIterations = 20;
    dynamicsWorld->getSolverInfo().m_splitImpulse = true;
    gContactAddedCallback = adjustTrackEdgeContacts;

    // Ground plane
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0), -1000);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo groundCI(0, groundMotionState, groundShape, btVector3(0,0,0));
    btRigidBody* groundBody = new btRigidBody(groundCI);
    dynamicsWorld->addRigidBody(groundBody);

}

btRigidBody* Collisions::triangleShapeS(SceneObject& sceneObj, const MeshData& mesh) {
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(sceneObj.position.x, sceneObj.position.y, sceneObj.position.z));
    transform.setRotation(btQuaternion(sceneObj.rotation.x,sceneObj.rotation.y,sceneObj.rotation.z,sceneObj.rotation.w));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

    btTriangleMesh* triMesh = new btTriangleMesh();
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        int i0 = mesh.indices[i] * 8;
        int i1 = mesh.indices[i+1] * 8;
        int i2 = mesh.indices[i+2] * 8;

        btVector3 v0(mesh.vertices[i0], mesh.vertices[i0+1], mesh.vertices[i0+2]);
        btVector3 v1(mesh.vertices[i1], mesh.vertices[i1+1], mesh.vertices[i1+2]);
        btVector3 v2(mesh.vertices[i2], mesh.vertices[i2+1], mesh.vertices[i2+2]);

        triMesh->addTriangle(v0, v1, v2);
    }

    btBvhTriangleMeshShape* triShape = new btBvhTriangleMeshShape(triMesh, true);
    auto* triangleInfoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo(triShape, triangleInfoMap);
    triShape->setTriangleInfoMap(triangleInfoMap);

    btScalar mass = 0.0f; // static object
    btVector3 inertia(0,0,0);

    btRigidBody::btRigidBodyConstructionInfo rInfo(mass, motionState, triShape, inertia);
    sceneObj.body = new btRigidBody(rInfo);
    sceneObj.body->setCollisionFlags(
        sceneObj.body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    dynamicsWorld->addRigidBody(sceneObj.body);

    return sceneObj.body;
}

void Collisions::update(float dt) {
    if (dynamicsWorld) {
        const btScalar frameTime = std::clamp(btScalar(dt), btScalar(0), btScalar(1.0 / 30.0));
        dynamicsWorld->stepSimulation(frameTime, 8, btScalar(1.0 / 120.0));
    }
}

void Collisions::removeRigidBody(btRigidBody* body) {
    if (dynamicsWorld == nullptr || body == nullptr) {
        return;
    }
    dynamicsWorld->removeRigidBody(body);
    delete body->getCollisionShape();
    delete body->getMotionState();
    delete body;
}

bool Collisions::isGrounded(const btRigidBody* body) const {
    if (dynamicsWorld == nullptr || body == nullptr) {
        return false;
    }

    const int manifoldCount = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < manifoldCount; ++i) {
        const btPersistentManifold* manifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const bool isBody0 = manifold->getBody0() == body;
        if (!isBody0 && manifold->getBody1() != body) {
            continue;
        }

        for (int j = 0; j < manifold->getNumContacts(); ++j) {
            const btManifoldPoint& contact = manifold->getContactPoint(j);
            const btScalar normalY = isBody0
                ? contact.m_normalWorldOnB.y()
                : -contact.m_normalWorldOnB.y();
            if (contact.getDistance() <= btScalar(0.05) && normalY > btScalar(0.5)) {
                return true;
            }
        }
    }
    return false;
}

glm::vec3 Collisions::getGroundNormal(const btRigidBody* body) const {
    if (dynamicsWorld == nullptr || body == nullptr) {
        return glm::vec3(0.0f);
    }

    struct ContactNormal {
        glm::vec3 point;
        glm::vec3 normal;
    };
    std::vector<ContactNormal> contacts;
    const int manifoldCount = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < manifoldCount; ++i) {
        const btPersistentManifold* manifold =
            dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const bool isBody0 = manifold->getBody0() == body;
        if (!isBody0 && manifold->getBody1() != body) {
            continue;
        }

        for (int j = 0; j < manifold->getNumContacts(); ++j) {
            const btManifoldPoint& contact = manifold->getContactPoint(j);
            if (contact.getDistance() > btScalar(0.12)) {
                continue;
            }
            const btVector3 normal = isBody0
                ? contact.m_normalWorldOnB
                : -contact.m_normalWorldOnB;
            if (normal.y() > btScalar(0.2)) {
                const btVector3 point = isBody0
                    ? contact.getPositionWorldOnA()
                    : contact.getPositionWorldOnB();
                contacts.push_back({
                    glm::vec3(point.x(), point.y(), point.z()),
                    glm::vec3(normal.x(), normal.y(), normal.z())
                });
            }
        }
    }
    if (contacts.empty()) {
        return glm::vec3(0.0f);
    }

    btTransform bodyTransform;
    body->getMotionState()->getWorldTransform(bodyTransform);
    const btVector3 bodyOrigin = bodyTransform.getOrigin();
    const glm::vec3 center(bodyOrigin.x(), bodyOrigin.y(), bodyOrigin.z());
    const auto nearestContact = std::min_element(
        contacts.begin(), contacts.end(),
        [&center](const ContactNormal& left, const ContactNormal& right) {
            const glm::vec3 leftOffset = left.point - center;
            const glm::vec3 rightOffset = right.point - center;
            return glm::dot(leftOffset, leftOffset) < glm::dot(rightOffset, rightOffset);
        });
    constexpr float neighborhoodRadius = 2.5f;
    glm::vec3 averagedNormal(0.0f);
    for (const auto& contact : contacts) {
        const float distance = glm::length(contact.point - nearestContact->point);
        if (distance <= neighborhoodRadius) {
            const float weight = 1.0f - distance / neighborhoodRadius;
            averagedNormal += contact.normal * weight;
        }
    }
    return glm::dot(averagedNormal, averagedNormal) > 0.0001f
               ? glm::normalize(averagedNormal)
               : glm::vec3(0.0f);
}

btRigidBody* Collisions::convexShapeD(SceneObject& sceneObj, const MeshData& mesh,
                                     const glm::vec3& scale) {

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(sceneObj.position.x, sceneObj.position.y, sceneObj.position.z));
    transform.setRotation(btQuaternion(sceneObj.rotation.x, sceneObj.rotation.y,
                                       sceneObj.rotation.z, sceneObj.rotation.w));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

    btConvexHullShape* hull = new btConvexHullShape();
    for (size_t i = 0; i < mesh.vertices.size(); i += 8) {
        btVector3 point(mesh.vertices[i] * scale.x, mesh.vertices[i + 1] * scale.y,
                        mesh.vertices[i + 2] * scale.z);
        hull->addPoint(point);
    }
    hull->initializePolyhedralFeatures();

    btScalar mass = 20.0f;
    btVector3 inertia(0,0,0);
    hull->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, hull, inertia);
    sceneObj.body = new btRigidBody(rbInfo);
    sceneObj.body->setFriction(0.8f);
    sceneObj.body->setRestitution(1.0f);
    sceneObj.body->setAngularFactor(btVector3(1,1,1));
    dynamicsWorld->addRigidBody(sceneObj.body);
    if (mesh.vertices.size() % 8 != 0) {
        std::cerr << "Vertex array misaligned!" << std::endl;
    }
    return sceneObj.body;
}

btRigidBody* Collisions::boxShapeD(SceneObject& sceneObj, const MeshData& mesh,
                                   const glm::vec3& scale) {
    if (mesh.vertices.empty()) {
        return nullptr;
    }

    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 8) {
        const glm::vec3 point(mesh.vertices[i] * scale.x,
                              mesh.vertices[i + 1] * scale.y,
                              mesh.vertices[i + 2] * scale.z);
        minimum = glm::min(minimum, point);
        maximum = glm::max(maximum, point);
    }

    const glm::vec3 halfExtents = (maximum - minimum) * 0.5f;
    if (halfExtents.x <= 0.0f || halfExtents.y <= 0.0f || halfExtents.z <= 0.0f) {
        std::cerr << "Cannot create car collision box from degenerate mesh bounds" << std::endl;
        return nullptr;
    }

    auto* shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(sceneObj.position.x, sceneObj.position.y, sceneObj.position.z));
    transform.setRotation(btQuaternion(sceneObj.rotation.x, sceneObj.rotation.y,
                                       sceneObj.rotation.z, sceneObj.rotation.w));

    btVector3 inertia(0.0f, 0.0f, 0.0f);
    constexpr btScalar mass = 20.0f;
    shape->calculateLocalInertia(mass, inertia);
    auto* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, inertia);
    sceneObj.body = new btRigidBody(info);
    sceneObj.body->setFriction(0.8f);
    sceneObj.body->setRestitution(0.0f);
    sceneObj.body->setCcdMotionThreshold(0.5f);
    sceneObj.body->setCcdSweptSphereRadius(0.15f);
    dynamicsWorld->addRigidBody(sceneObj.body);
    return sceneObj.body;
}
