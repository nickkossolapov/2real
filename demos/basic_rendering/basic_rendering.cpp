#include "engine/fps_logger.h"
#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/vec3.h"
#include "render/context.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "render/texture_loader.h"
#include "render/viewport.h"
#include "scene/camera.h"
#include "scene/mesh.h"

#include <algorithm>
#include <cmath>

namespace {

void update(const float dt, scene::Entity& entity, const input::State& input, scene::Camera& camera) {
  // Forward/back movement
  camera.position.x += input.move_y * -std::sin(camera.rotation.y);
  camera.position.z += input.move_y * std::cos(camera.rotation.y);
  camera.position.y += input.move_y * std::sin(camera.rotation.x);

  // Left/right movement
  camera.position.x += input.move_x * std::cos(camera.rotation.y);
  camera.position.z += input.move_x * std::sin(camera.rotation.y);

  camera.position.y += input.trigger_right - input.trigger_left;

  camera.rotation.x += input.look_y;
  camera.rotation.y += -input.look_x;

  // Clamp pitch, wrap yaw
  constexpr float max_pitch = math::deg_to_rad(89.0f);
  camera.rotation.x = std::clamp(camera.rotation.x, -max_pitch, max_pitch);
  camera.rotation.y = std::fmod(camera.rotation.y, 2.0f * std::numbers::pi_v<float>);
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr bool enable_v_sync = true;

  engine::SdlContext sdl;
  if (auto err = sdl.init(enable_v_sync); err != engine::InitError::None) {
    return static_cast<int>(err);
  }

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(engine::window::width) / static_cast<float>(engine::window::height);

  scene::Camera camera = {.fov = fov, .aspect_ratio = aspect, .z_near = 0.1f, .z_far = 100.0f, .position = {0, 0, 0}};

  auto test_mesh = render::load_obj_file("./assets/f22.obj");
  const auto test_texture = render::load_texture_file("./assets/f22.png");

  if (!test_mesh.has_value()) {
    SDL_Log("Failed to load mesh");

    return -1;
  }

  auto cube_mesh_ptr = std::make_shared<scene::Mesh>(std::move(*test_mesh));

  scene::Entity test_entity = {.mesh = cube_mesh_ptr};

  test_entity.transform.position.z = 5;

  if (!test_texture.has_value()) {
    SDL_Log("Failed to load texture");

    return -1;
  }

  test_entity.texture = std::make_shared<render::Texture>(test_texture.value());

  test_entity.transform.position.z = 5.0f;

  const auto light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f});

  bool quit = false;
  constexpr render::Viewport viewport = {engine::window::width, engine::window::height};

  constexpr uint32_t blue = 0xFF87CEEB;
  constexpr uint32_t black = 0xFF000000;

  auto renderer = render::Context(viewport.width, viewport.height, black);
  auto frame_limiter = engine::FrameLimiter(60, enable_v_sync);

  input::State input_state;

  engine::FpsLogger fps_logger;

  while (!quit) {
    const float dt = frame_limiter.tick() / 1000.0f; // Convert to seconds

    quit = input::process_input(dt, sdl.gamepad(), input_state);
    update(dt, test_entity, input_state, camera);

    render::pipeline::render_entity(renderer, viewport, test_entity, camera, light, render::RenderMode::Textured);

    renderer.present(sdl.renderer(), sdl.display_texture());

    fps_logger.tick();
  }

  return 0;
}
