//
// Created by 777ni on 2026/09/15.
//

#include "Collisions.h"
#include <bullet/btBulletDynamicsCommon.h>

// Keep these static so they live across calls
static btBroadphaseInterface* broadphase;
static btDefaultCollisionConfiguration* collisionConfiguration;
static btCollisionDispatcher* dispatcher;
static btSequentialImpulseConstraintSolver* solver;
static btDiscreteDynamicsWorld* dynamicsWorld;

void Collisions::init() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

    // Ground plane
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0), -20);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo groundCI(0, groundMotionState, groundShape, btVector3(0,0,0));
    btRigidBody* groundBody = new btRigidBody(groundCI);
    dynamicsWorld->addRigidBody(groundBody);

}

btRigidBody* Collisions::convexShapeS(SceneObject& sceneObj, const MeshData& mesh) {
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(sceneObj.position.x, sceneObj.position.y, sceneObj.position.z));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

    btConvexHullShape* hull = new btConvexHullShape();
    for (size_t i = 0; i < mesh.vertices.size(); i += 8) {
        btVector3 point(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
        hull->addPoint(point);
    }

    hull->initializePolyhedralFeatures();
    btScalar mass = 0.0f;
    btVector3 inertia(0,0,0);

    btRigidBody::btRigidBodyConstructionInfo rInfo(mass, motionState, hull, inertia);
    sceneObj.body = new btRigidBody(rInfo);

    dynamicsWorld->addRigidBody(sceneObj.body);

    return sceneObj.body;
}

void Collisions::update(float dt) {
    if (dynamicsWorld) {
        dynamicsWorld->stepSimulation(dt, 10);
    }
}

btRigidBody* Collisions::convexShapeD(SceneObject& sceneObj, const MeshData& mesh) {

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(sceneObj.position.x, sceneObj.position.y, sceneObj.position.z));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

    btConvexHullShape* hull = new btConvexHullShape();
    for (size_t i = 0; i < mesh.vertices.size(); i += 8) {
        btVector3 point(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
        hull->addPoint(point);
    }
    hull->initializePolyhedralFeatures();

    btScalar mass = 20.0f;
    btVector3 inertia(0,0,0);
    hull->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, hull, inertia);
    sceneObj.body = new btRigidBody(rbInfo);
    sceneObj.body->setFriction(5.0f);
    sceneObj.body->setRestitution(1.0f);
    sceneObj.body->setAngularFactor(btVector3(1,1,1));
    dynamicsWorld->addRigidBody(sceneObj.body);
    if (mesh.vertices.size() % 8 != 0) {
        std::cerr << "Vertex array misaligned!" << std::endl;
    }
    return sceneObj.body;
}
