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

void Collisions::update(float dt) {
    if (dynamicsWorld) {
        dynamicsWorld->stepSimulation(dt, 10);
    }
}

btRigidBody* Collisions::spawnCube(glm::vec3 pos) {
    btCollisionShape* cubeShape = new btBoxShape(btVector3(0.5f,0.5f,0.5f));
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(pos.x, pos.y, pos.z)));
    btScalar mass = 1.0f;
    btVector3 inertia(0,0,0);
    cubeShape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, cubeShape, inertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setFriction(1.0f);
    body->setRestitution(1.0f);
    body->setAngularFactor(btVector3(1,1,1));
    dynamicsWorld->addRigidBody(body);
    return body;
}
