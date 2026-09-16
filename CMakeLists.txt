cmake_minimum_required(VERSION 3.20)
project(Physics_sim)

set(CMAKE_CXX_STANDARD 23)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Bullet CONFIG REQUIRED)

find_path(TINYGLTF_INCLUDE_DIR tiny_gltf.h
        PATHS ${VCPKG_ROOT}/installed/x64-mingw-dynamic/include
)

add_executable(Physics_sim
        src/main.cpp
        Meshes/Meshes.cpp
        Rendering/VAO_VBO.cpp
        Shader/Shader.cpp
        Loaders/Loaders.cpp
        Math/matrix.cpp
        Math/gravity.cpp
        Camera/camera.cpp
        Math/Collisions.cpp
)

target_link_libraries(Physics_sim PRIVATE
        glfw
        glad::glad
        BulletDynamics
        BulletCollision
        LinearMath
)

target_include_directories(Physics_sim PRIVATE
    ${Stb_INCLUDE_DIR}
        ${TINYGLTF_INCLUDE_DIR}
)
