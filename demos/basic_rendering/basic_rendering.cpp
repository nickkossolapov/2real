#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "render/context.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "render/texture_loader.h"
#include "render/viewport.h"
#include "scene/camera.h"
#include "scene/mesh.h"

namespace {

void update(const float dt, scene::Entity& entity, const input::State& input) {
  // entity.transform.rotation.x = input.move_x * std::numbers::pi;
  // entity.transform.rotation.y = -input.look_x * std::numbers::pi;
  // entity.transform.rotation.z = -input.move_y * std::numbers::pi;
  // entity.transform.rotation += dt * 0.0003f;
  entity.transform.rotation.y += dt * 0.0003f;
  // entity.transform.scale += dt * 0.0001f;
  // entity.transform.position.x = input.look_y;
  // entity.transform.position.y += dt * 0.0001f;
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr bool enable_v_sync = true;

  engine::SdlContext sdl;
  if (auto err = sdl.init(enable_v_sync); err != engine::InitError::None) {
    return static_cast<int>(err);
  }

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(engine::window::height) / static_cast<float>(engine::window::width);
  const scene::Camera camera = {.projection = math::mat4::perspective(fov, aspect, 0.1f, 100.0f)};

  auto cube_mesh = render::load_obj_file("./assets/crab.obj");

  if (!cube_mesh.has_value()) {
    SDL_Log("Failed to load mesh");

    return -1;
  }

  auto cube_mesh_ptr = std::make_shared<scene::Mesh>(std::move(*cube_mesh));

  scene::Entity test_entity = {.mesh = cube_mesh_ptr};

  test_entity.transform.position.z = 5;

  const auto test_texture = render::load_texture_file("./assets/crab.png");

  if (!test_texture.has_value()) {
    SDL_Log("Failed to load texture");

    return -1;
  }

  test_entity.texture = std::make_shared<render::Texture>(test_texture.value());

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
