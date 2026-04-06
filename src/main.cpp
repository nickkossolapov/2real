#include "engine/timing.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/graphics.h"

#include <cmath>
#include <ranges>
#include <vector>
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

Vec2 project(const Vec3 point, const Vec3 camera_pos) {
  return {
      point.x / (point.z - camera_pos.z),
      point.y / (point.z - camera_pos.z)
  };
}

void draw_rect(graphics::Context& renderer,
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

std::vector<Vec3> make_cube() {
  std::vector<Vec3> cube_points(9 * 9 * 9);
  int p = 0;

  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        cube_points[p++] = {x, y, z};
      }
    }
  }

  return cube_points;
}

void update(float dt, Vec3& cube_rotation) {
  constexpr float rotate_speed = 0.0005f;

  cube_rotation.x += dt * rotate_speed;
  cube_rotation.y += dt * rotate_speed;
  cube_rotation.z += dt * rotate_speed;
}

void render_scene(graphics::Context& renderer, std::vector<Vec3>& cube_points, Vec3 camera_pos, Vec3 cube_rotation) {
  const auto projected_points =
      cube_points
      | std::views::transform([cube_rotation](const Vec3 v) {
        auto transformed_point = vec3::rotate_x(v, cube_rotation.x);
        transformed_point = vec3::rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3::rotate_z(transformed_point, cube_rotation.z);

        return transformed_point;
      })
      | std::views::transform([camera_pos](const Vec3 v) { return project(v, camera_pos); });

  for (auto p : projected_points) {
    constexpr auto fov_factor = 640;

    draw_rect(
        renderer,
        fov_factor * p.x + graphics::window::width / 2,
        fov_factor * p.y + graphics::window::height / 2,
        4,
        4,
        0xFFFFFF00
        );
  }
}

} // namespace

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* display_texture = nullptr;

  constexpr bool enable_v_sync = true;

  if (auto init_res = graphics::init_sdl(window, sdl_renderer, display_texture, enable_v_sync);
    init_res != graphics::InitError::None) {
    return static_cast<int>(init_res);
  }

  bool quit = false;
  auto renderer = graphics::Context(graphics::window::width, graphics::window::height);
  auto frame_limiter = FrameLimiter(60, enable_v_sync);

  Vec3 camera_pos = {0, 0, -5};
  Vec3 cube_rotation = {0, 0, 0};
  auto cube_points = make_cube();

  while (!quit) {
    quit = process_input();
    const float dt = frame_limiter.tick();

    update(dt, cube_rotation);

    render_scene(renderer, cube_points, camera_pos, cube_rotation);
    renderer.present(*sdl_renderer, *display_texture);
  }

  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
