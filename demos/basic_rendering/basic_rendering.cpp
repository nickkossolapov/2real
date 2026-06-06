#include "engine/fps_logger.h"
#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/fixed.h"
#include "math/rect.h"
#include "math/vec2_fixed.h"
#include "math/vec3.h"
#include "render/context.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "render/texture_loader.h"
#include "render/viewport.h"
#include "scene/camera.h"
#include "scene/camera_controller.h"
#include "scene/mesh.h"

#include <algorithm>
#include <format>

namespace {

void update(const float dt, const input::State& input, scene::Camera& camera) {
  update_fps_camera(camera, input);
}

std::optional<scene::Entity> create_entity(const std::string& name) {
  const auto mesh = render::load_obj_file(std::format("./assets/{0}.obj", name));
  const auto texture = render::load_texture_file(std::format("./assets/{0}.png", name));

  if (!mesh.has_value()) {
    SDL_Log("Failed to load mesh for %s", name.data());

    return {};
  }

  scene::Entity entity = {.mesh = std::make_shared<scene::Mesh>(std::move(*mesh))};

  if (!texture.has_value()) {
    SDL_Log("Failed to load texture for %s", name.data());

    return {};
  }

  entity.texture = std::make_shared<render::Texture>(texture.value());
  entity.flat_colour = 0xFFFFFFFF;

  return entity;
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

  const auto light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f});

  bool quit = false;
  constexpr render::Viewport viewport = {engine::window::width, engine::window::height};

  constexpr uint32_t blue = 0xFF87CEEB;

  auto f22 = create_entity("f22").value();
  auto f117 = create_entity("f117").value();
  auto efa = create_entity("efa").value();
  auto runway = create_entity("runway").value();

  camera.position.x = 1;
  camera.position.y = 2;
  camera.position.z = 6;
  camera.rotation.y = math::deg_to_rad(170);
  camera.rotation.x = math::deg_to_rad(-20);

  f22.transform.position.z = 0;
  f22.transform.rotation.y = math::deg_to_rad(90);

  f117.transform.position.z = 1;
  f117.transform.position.x = 2;
  f117.transform.rotation.y = math::deg_to_rad(120);

  efa.transform.position.z = 1;
  efa.transform.position.x = -2;
  efa.transform.rotation.y = math::deg_to_rad(60);

  runway.transform.position.y = -0.35;

  std::vector entities{f22, f117, efa, runway};

  std::vector<math::Vec2> vertices = {
      {40, 40},
      {80, 40},
      {40, 80},
      {90, 90},
      {75, 20},
  };

  auto renderer = render::Context(viewport.width, viewport.height, blue);
  auto frame_limiter = engine::FrameLimiter(60, enable_v_sync);

  input::State input_state;

  engine::FpsLogger fps_logger;

  while (!quit) {
    const float dt = frame_limiter.tick() / 1000.0f; // Convert to seconds

    quit = input::process_input(dt, sdl.gamepad(), input_state);
    update(dt, input_state, camera);

    render::pipeline::render_entity(renderer, viewport, f22, camera, light, render::RenderMode::Flat);

    // triangle_fill(renderer, std::array{vertices[0], vertices[1], vertices[2]});

    renderer.present(sdl.renderer(), sdl.display_texture());

    fps_logger.tick();
  }

  return 0;
}
