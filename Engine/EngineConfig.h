#ifndef PHYSICS_SIM_ENGINE_CONFIG_H
#define PHYSICS_SIM_ENGINE_CONFIG_H

#include <glm/glm.hpp>

namespace engine_config {

inline const glm::vec3 cockpitOffset{0.0f, 0.15f, 0.047f};
inline const glm::vec3 chassisOffset{0.0f, -0.3f, 0.0f};
inline const glm::vec3 chassisHalfExtents{0.72f, 0.22f, 1.55f};

}

#endif
