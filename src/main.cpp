#include "math/vec2.h"
#include "math/vec3.h"
#include "renderer/renderer.h"

#include <cmath>
#include <ranges>
#include <vector>
#include <SDL3/SDL.h>

namespace {

bool handle_events() {
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

Vec2 project(Vec3 point) {
  return {
      point.x / point.z,
      point.y / point.z
  };
}

void draw_rect(renderer::Renderer& renderer,
               const int x,
               const int y,
               const int width,
               const int height,
               const uint32_t color) {
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      renderer.draw_pixel(i, j, color);
    }
  }
}

} // namespace

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* display_texture = nullptr;

  if (auto init_res = renderer::init(window, sdl_renderer, display_texture); init_res != renderer::InitError::None) {
    return static_cast<int>(init_res);
  }

  bool quit = false;
  auto renderer = renderer::Renderer(renderer::window::width, renderer::window::height);

  const Vec3 camera_pos = {0, 0, -5};

  std::vector<Vec3> cube_points(9 * 9 * 9);
  int i = 0;

  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        cube_points[i++] = {x, y, z - camera_pos.z};
      }
    }
  }

  while (!quit) {
    quit = handle_events();

    for (auto projected_points = cube_points | std::views::transform(project); auto p : projected_points) {
      auto const fov_factor = 640;

      draw_rect(
          renderer,
          fov_factor * p.x + renderer::window::width / 2,
          fov_factor * p.y + renderer::window::height / 2,
          4,
          4,
          0xFFFFFF00
          );
    }

    renderer.present(*sdl_renderer, *display_texture);
  }

  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
