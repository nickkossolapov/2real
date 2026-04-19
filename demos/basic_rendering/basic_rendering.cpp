#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "render/graphics.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "scene/camera.h"
#include "scene/mesh.h"

#include <algorithm>
#include <SDL3/SDL.h>

namespace {

void update(const float dt, scene::Entity& entity, const input::State& input) {
  entity.transform.rotation.x = input.move_x * std::numbers::pi;
  entity.transform.rotation.y = input.look_x * std::numbers::pi;
  entity.transform.rotation.z = input.move_y * std::numbers::pi;
  // entity.transform.rotation += dt * 0.0002f;
  // entity.transform.scale += dt * 0.0001f;
  entity.transform.position.x = input.look_y;
  // entity.transform.position.y += dt * 0.0001f;
}

void destroy(SDL_Window* window, SDL_Renderer* sdl_renderer, SDL_Gamepad* controller) {
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(window);
  SDL_CloseGamepad(controller);
  SDL_Quit();
}

}

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* display_texture = nullptr;
  SDL_Gamepad* controller = nullptr;
  input::State input_state;

  constexpr
      bool enable_v_sync = true;

  if (auto init_res = engine::init_sdl(window, sdl_renderer, display_texture, controller, enable_v_sync);
    init_res != engine::InitError::None) {

    return static_cast<int>(init_res);
  }

  const auto test_mesh = std::make_shared<scene::Mesh>();
  load_obj_file("./basic_rendering/assets/f22.obj", *test_mesh);

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(engine::window::height) / static_cast<float>(engine::window::width);
  const scene::Camera camera = {.projection = math::mat4::perspective(fov, aspect, 0.1, 100.0)};

  scene::Entity test_entity = {
      .mesh = test_mesh
  };

  test_entity.transform.position.z = 5;

  const auto light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f});

  bool quit = false;
  auto renderer = graphics::Context(engine::window::width, engine::window::height);
  auto frame_limiter = FrameLimiter(60, enable_v_sync);

  while (!quit) {
    const float dt = frame_limiter.tick();

    quit = input::process_input(dt, input_state);
    update(dt, test_entity, input_state);

    render::pipeline::render_entity(renderer, test_entity, camera, light);
    renderer.present(*sdl_renderer, *display_texture);
  }

  destroy(window, sdl_renderer, controller);

  return 0;
}
