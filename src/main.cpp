#include "engine/timing.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/draw.h"
#include "render/graphics.h"
#include "render/triangle.h"

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

Vec2 project(const Vec3& p, const Vec3 camera_pos) {
  constexpr int fov_factor = 640;

  return {
      fov_factor * p.x / (p.z - camera_pos.z) + graphics::window::width / 2,
      fov_factor * p.y / (p.z - camera_pos.z) + graphics::window::height / 2
  };
}


std::vector<Vec3> cube_vertices = {
    {-1, -1, -1},
    {-1, 1, -1},
    {1, 1, -1},
    {1, -1, -1},
    {1, 1, 1},
    {1, -1, 1},
    {-1, 1, 1},
    {-1, -1, 1}
};

std::vector<Face> cube_faces = {
    // front
    {1, 2, 3},
    {1, 3, 4},
    // right
    {4, 3, 5},
    {4, 5, 6},
    // back
    {6, 5, 7},
    {6, 7, 8},
    // left
    {8, 7, 2},
    {8, 2, 1},
    // top
    {2, 7, 5},
    {2, 5, 3},
    // bottom
    {6, 8, 1},
    {6, 1, 4}
};

std::vector<Triangle> make_cube() {
  auto view =
      cube_faces
      | std::views::transform([](const Face face) {
        return Triangle(cube_vertices[face.a - 1], cube_vertices[face.b - 1], cube_vertices[face.c - 1]);
      });

  return std::vector(view.begin(), view.end());
}

void update(const float dt, Vec3& cube_rotation) {
  constexpr float rotate_speed = 0.0005f;

  cube_rotation.x += dt * rotate_speed;
  cube_rotation.y += dt * rotate_speed;
  cube_rotation.z += dt * rotate_speed;
}


void render_scene(graphics::Context& context,
                  const std::vector<Triangle>& cube_triangles,
                  const Vec3 camera_pos,
                  const Vec3 cube_rotation) {
  for (const Triangle& t : cube_triangles) {
    const Vec2 a = project(vec3::rotate(t.a, cube_rotation), camera_pos);
    const Vec2 b = project(vec3::rotate(t.b, cube_rotation), camera_pos);
    const Vec2 c = project(vec3::rotate(t.c, cube_rotation), camera_pos);

    draw::line(context, a.x, a.y, b.x, b.y, 0xFFFFFF00);
    draw::line(context, b.x, b.y, c.x, c.y, 0xFFFFFF00);
    draw::line(context, c.x, c.y, a.x, a.y, 0xFFFFFF00);
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
  const auto cube_points = make_cube();

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
