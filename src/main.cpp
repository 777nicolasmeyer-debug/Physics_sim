#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Meshes/Meshes.h"
#include "../Meshes/Shader.h"
#include "VAO_VBO.h"
#include "Loaders.h"
#include "matrix.h"


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Shader shader;
Meshes::Cube cube;
VAO_VBO buffers;
Loaders loader;
Matrix matrix(WIDTH, HEIGHT);

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

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    buffers.setCubeVBO(cube);
    buffers.setCubeVAO(cube);

    unsigned int crateTex = loader.loadImageFromFile("../assets/crate.png");
    shader.createShaders("../Shader/vertex.glsl", "../Shader/fragment.glsl");
    shader.use();

    shader.loadTexture("tex", 0);


    matrix.Translate(glm::vec3(0.0f, 0.0f, -3.0f));
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    auto lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.loadMatrix("model", matrix.model);
        shader.loadMatrix("projection", matrix.projection);
        shader.loadMatrix("view", matrix.view);
        matrix.Rotate(50.0f * deltaTime,glm::vec3(0.5f, 0.5f, 1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crateTex);
        glBindVertexArray(buffers.vao);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(cube.vertices.size() / 5));

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}