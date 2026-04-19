#include "engine/timing.h"
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

bool process_input() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      return true;
    }

    if (e.type == SDL_EVENT_KEY_DOWN) {
      if (e.key.key == SDLK_ESCAPE) {
        return true;
      }
    }
  }

  return false;
}

void update(const float dt, scene::Entity& entity) {
  entity.transform.rotation += dt * 0.0005f;
  // entity.transform.scale += dt * 0.0001f;
  // entity.transform.position.x += dt * 0.0001f;
  // entity.transform.position.y += dt * 0.0001f;
}

} // namespace

void destroy(SDL_Window* window, SDL_Renderer* sdl_renderer) {
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* display_texture = nullptr;

  constexpr bool enable_v_sync = true;

  if (auto init_res = graphics::init_sdl(window, sdl_renderer, display_texture, enable_v_sync);
    init_res != graphics::InitError::None) {

    return static_cast<int>(init_res);
  }

  const auto test_mesh = std::make_shared<scene::Mesh>();
  load_obj_file("./assets/f22.obj", *test_mesh);

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(graphics::window::height) / static_cast<float>(graphics::window::width);
  const scene::Camera camera = {.projection = math::mat4::perspective(fov, aspect, 0.1, 100.0)};

  scene::Entity test_entity = {
      .mesh = test_mesh
  };

  test_entity.transform.position.z = 5;

  bool quit = false;
  auto renderer = graphics::Context(graphics::window::width, graphics::window::height);
  auto frame_limiter = FrameLimiter(60, enable_v_sync);

  while (!quit) {
    quit = process_input();
    const float dt = frame_limiter.tick();

    update(dt, test_entity);

    render::pipeline::render_entity(renderer, test_entity, camera);
    renderer.present(*sdl_renderer, *display_texture);
  }

  destroy(window, sdl_renderer);

  return 0;
}
