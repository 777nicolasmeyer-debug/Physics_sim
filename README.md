# Physics_sim

`Engine` is a renderer/physics component; it does not create a window, poll events,
or swap buffers. The host application owns the OpenGL context and drives the engine,
so it can use GLFW, Qt, or an ImGui host:

1. Create and activate an OpenGL 3.3 context, then initialize GLAD.
2. Call `Engine::initialize()` once while that context is current.
3. Add model files with `addStaticModel`, `addDynamicModel`, and `addLight`.
   The engine loads all meshes in the selected glTF scene, applies node transforms,
   and caches loaded model data. Mesh-level methods are also available for custom
   procedural assets.
4. Each frame, translate the host's keyboard state to `Engine::CameraInput`, call
   `handleKeyboardInput`, `update(deltaTime)`, and `render(width, height)`.
5. Draw the GUI after `render`, then let the host swap/present its buffers.

`src/main.cpp` is the GLFW example host and contains the sample game's asset setup
and spawn-key bindings.

`WASD` movement

`1 - 9` used to load models into world
`P` to toggle Player

`L` to spawn lights

A Qt host can use the same engine calls from its
`QOpenGLWidget` lifecycle without adopting the sample application's GLFW loop.
