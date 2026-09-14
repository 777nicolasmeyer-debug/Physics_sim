//
// Created by 777ni on 2026/09/14.
//

#include "collisions.h"

#include <glm/ext/quaternion_trigonometric.hpp>

bool Collisions::BoxBoxCollision(OBB &box1, OBB &box2) {
    glm::vec3 axes[15];
    const glm::vec3 r1 = box1.right * box1.halfSize.x;
    const glm::vec3 u1 = box1.up * box1.halfSize.y;
    const glm::vec3 f1 = box1.forward * box1.halfSize.z;

    glm::vec3 r2 = box2.right * box2.halfSize.x;
    glm::vec3 u2 = box2.up * box2.halfSize.y;
    glm::vec3 f2 = box2.forward * box2.halfSize.z;

    glm::vec3 verticesBox1[8] = {
        box1.center + r1 + u1 + f1,
        box1.center + r1 + u1 - f1,

        box1.center + r1 - u1 + f1,
        box1.center + r1 - u1 - f1,

        box1.center - r1 + u1 + f1,
        box1.center - r1 + u1 - f1,

        box1.center - r1 - u1 + f1,
        box1.center - r1 - u1 - f1
    };

    glm::vec3 verticesBox2[8] = {
        box2.center + r2 + u2 + f2,
        box2.center + r2 + u2 - f2,

        box2.center + r2 - u2 + f2,
        box2.center + r2 - u2 - f2,

        box2.center - r2 + u2 + f2,
        box2.center - r2 + u2 - f2,

        box2.center - r2 - u2 + f2,
        box2.center - r2 - u2 - f2
    };

    axes[0] = box1.right;
    axes[1] = box1.up;
    axes[2] = box1.forward;

    axes[3] = box2.right;
    axes[4] = box2.up;
    axes[5] = box2.forward;

    axes[6] = glm::cross(box1.right, box2.right);
    axes[7] = glm::cross(box1.right, box2.up);
    axes[8] = glm::cross(box1.right, box2.forward);

    axes[9] = glm::cross(box1.up, box2.right);
    axes[10] = glm::cross(box1.up, box2.up);
    axes[11] = glm::cross(box1.up, box2.forward);

    axes[12] = glm::cross(box1.forward, box2.right);
    axes[13] = glm::cross(box1.forward, box2.up);
    axes[14] = glm::cross(box1.forward, box2.forward);

    float smallestOverlap = FLT_MAX;
    glm::vec3 collisionNormal;
    float overlap;

    for (int i = 0; i < 15; i++) {
        glm::vec3 axis = axes[i];

        if (glm::length(axis) < 0.0001f) {
            continue;
        }

        axis = glm::normalize(axis);

        float minBox1 = glm::dot(verticesBox1[0], axis);
        float maxBox1 = minBox1;

        for (int j = 1; j < 8; j++) {
            float projection = glm::dot(verticesBox1[j], axis);

            minBox1 = std::min(minBox1, projection);
            maxBox1 = std::max(maxBox1, projection);
        }

        float minBox2 = glm::dot(verticesBox2[0], axis);
        float maxBox2 = minBox2;

        for (int j = 0; j < 8; j++) {
            float projection = glm::dot(verticesBox2[j], axis);
            minBox2 = std::min(minBox2, projection);
            maxBox2 = std::max(maxBox2, projection);
        }

        overlap = std::min(maxBox1, maxBox2) - std::max(minBox1, minBox2);

        glm::vec3 direction = box2.center - box1.center;

        if (glm::dot(direction, axis) < 0.0f)
        {
            axis = -axis;
        }

        if (overlap < smallestOverlap) {
            smallestOverlap = overlap;
            collisionNormal = axis;
        }

        if (maxBox1 < minBox2 || maxBox2 < minBox1) {
            return false;
        }
    }
    ResolveBoxBoxCollision(box1, box2, smallestOverlap, collisionNormal);;

    return true;
}

void Collisions::ResolveBoxBoxCollision(OBB& box1, OBB& box2, const float overlap, const glm::vec3& normal)
{
    if (box1.type == ColliderType::Plane) {
        box2.center += normal * overlap;
    }
    else if (box2.type == ColliderType::Plane) {
        box1.center -= normal * overlap;
    }
    else {
        glm::vec3 correction = normal * overlap * 0.5f;

        box1.center -= correction;
        box2.center += correction;
    }
}
