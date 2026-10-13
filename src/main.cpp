#include "../Engine/Engine.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>

namespace {

struct ApplicationState {
    Engine* engine = nullptr;
    bool spawnLightWasDown = false;
    bool spawnObjectWasDown = false;
    bool spawnCarWasDown = false;
    bool resetWasDown = false;
};

void mouseCallback(GLFWwindow* window, const double xpos, const double ypos) {
    auto* state = static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr && state->engine != nullptr) {
        state->engine->handleMouseInput(xpos, ypos);
    }
}

Engine::CameraInput readCameraInput(GLFWwindow* window) {
    return {
        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS,
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS,
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
    };
}

}

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics sim", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int result = 0;
    {
        Engine engine;
        ApplicationState app{&engine};
        glfwSetWindowUserPointer(window, &app);
        glfwSetCursorPosCallback(window, mouseCallback);

        if (!engine.initialize()) {
            result = 1;
        } else {
            engine.addStaticModel("../assets/SpaceStation_Color.glb", glm::vec3(0.0f));

            glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
            float lastFrame = static_cast<float>(glfwGetTime());
            while (!glfwWindowShouldClose(window)) {
                const float currentFrame = static_cast<float>(glfwGetTime());
                const float deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                engine.handleKeyboardInput(readCameraInput(window), deltaTime);

                const bool lightDown = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
                if (lightDown && !app.spawnLightWasDown) {
                    engine.addLight("../assets/Light.glb", engine.getCameraPosition());
                }
                app.spawnLightWasDown = lightDown;

                const bool objectDown = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
                if (objectDown && !app.spawnObjectWasDown) {
                    engine.addDynamicModel("../assets/Object1.glb", engine.getCameraPosition());
                }
                app.spawnObjectWasDown = objectDown;

                const bool carDown = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;
                if (carDown && !app.spawnCarWasDown) {
                    const glm::vec3 carPosition =
                        engine.getCameraPosition() + engine.getCameraForward() * 5.0f +
                        glm::vec3(0.0f, 2.0f, 0.0f);
                    engine.addDynamicModel("../assets/F1.glb", carPosition);
                }
                app.spawnCarWasDown = carDown;

                const bool resetDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
                if (resetDown && !app.resetWasDown) {
                    engine.clearDynamicObjects();
                    engine.clearLights();
                }
                app.resetWasDown = resetDown;

                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }

                engine.update(deltaTime);
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window, &width, &height);
                engine.render(width, height);

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
