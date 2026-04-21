#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "render/context.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "render/viewport.h"
#include "scene/camera.h"
#include "scene/mesh.h"

namespace {

void update(const float dt, scene::Entity& entity, const input::State& input) {
  // entity.transform.rotation.x = input.move_x * std::numbers::pi;
  // entity.transform.rotation.y = -input.look_x * std::numbers::pi;
  // entity.transform.rotation.z = -input.move_y * std::numbers::pi;
  entity.transform.rotation += dt * 0.0005f;
  // entity.transform.scale += dt * 0.0001f;
  entity.transform.position.x = input.look_y;
  // entity.transform.position.y += dt * 0.0001f;
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr bool enable_v_sync = true;

  engine::SdlContext sdl;
  if (auto err = sdl.init(enable_v_sync); err != engine::InitError::None) {
    return static_cast<int>(err);
  }

  const auto test_mesh = std::make_shared<scene::Mesh>();
  render::load_obj_file("./assets/cube.obj", *test_mesh);

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(engine::window::height) / static_cast<float>(engine::window::width);
  const scene::Camera camera = {.projection = math::mat4::perspective(fov, aspect, 0.1f, 100.0f)};

  scene::Entity test_entity = {.mesh = test_mesh};

  test_entity.transform.position.z = 5;

  const auto light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f});

  bool quit = false;
  constexpr render::Viewport viewport = {engine::window::width, engine::window::height};
  auto renderer = render::Context(viewport.width, viewport.height);
  auto frame_limiter = engine::FrameLimiter(60, enable_v_sync);

  input::State input_state;

  while (!quit) {
    const float dt = frame_limiter.tick();

    quit = input::process_input(dt, input_state);
    update(dt, test_entity, input_state);

    render::pipeline::render_entity(renderer, viewport, test_entity, camera, light);
    renderer.present(sdl.renderer(), sdl.display_texture());
  }

  return 0;
}
