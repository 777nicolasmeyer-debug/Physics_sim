# Physics_sim

`Engine` is a renderer/physics component; it does not create a window, poll events,
or swap buffers. The host application owns the OpenGL context and drives the engine,
so it can use GLFW, Qt, or an ImGui host.
From now on I'm going to focus on making this a F1 game so it's going to have specific stuf that not every other
game might need.

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
and spawn-key bindings. The reusable engine implementation and its supporting
modules are grouped under `Engine/`; `Engine/loaders_implementation.cpp` contains
the tinygltf/STB implementation definitions. A Qt host can use the same engine
calls from its `QOpenGLWidget` lifecycle without adopting the sample application's
GLFW loop.

## Source layout

- `src/` contains the GLFW example application.
- `Engine/` contains `Engine.h`, the tinygltf implementation unit, and the engine
  subsystems and their supporting `Camera/`, `Loaders/`, `Math/`, `Meshes/`,
  `Rendering/`, `Scene/`, and `Shader/` folders.
- `Engine/engine.cpp` owns engine startup, frame updates, and the main render pass.
- `Engine/engine_assets.cpp`, `Engine/engine_player.cpp`, `Engine/engine_scene.cpp`,
  and `Engine/engine_shadows.cpp` keep model loading, vehicle control, scene
  operations, and shadow rendering separate from the frame loop.
- `Engine/EngineConfig.h` centralizes cockpit and chassis offsets used by the player.
