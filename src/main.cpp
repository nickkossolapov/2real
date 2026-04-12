#include "engine/timing.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/draw.h"
#include "render/graphics.h"
#include "render/mesh.h"
#include "render/obj_loader.h"

#include <algorithm>
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
      fov_factor * (p.x - camera_pos.x) / (p.z - camera_pos.z) + graphics::window::width / 2,
      fov_factor * (p.y - camera_pos.y) / (p.z - camera_pos.z) + graphics::window::height / 2
  };
}

bool is_front_facing(const Triangle& triangle, const Vec3& camera_pos) {
  const Vec3 normal = vec3::cross(triangle.b - triangle.a, triangle.c - triangle.a);
  const Vec3 to_camera = camera_pos - triangle.a;

  return vec3::dot(normal, to_camera) > 0;
}

void sort_by_depth(std::vector<Triangle>& triangles) {
  auto comparer = [](const Triangle& l, const Triangle& r) {
    const float z_l = l.a.z + l.b.z + l.c.z;
    const float z_r = r.a.z + r.b.z + r.c.z;

    return z_l > z_r;
  };

  std::ranges::sort(triangles.begin(), triangles.end(), comparer);
}

std::vector<Triangle> transform_mesh(const Mesh& mesh, const Vec3& camera_pos) {
  auto make_triangle = [&mesh](const Face& face) {
    return Triangle(
        vec3::rotate(mesh.vertices[face.a], mesh.rotation),
        vec3::rotate(mesh.vertices[face.b], mesh.rotation),
        vec3::rotate(mesh.vertices[face.c], mesh.rotation));
  };
  auto check_back_face_culling = [&camera_pos](const Triangle& t) { return is_front_facing(t, camera_pos); };

  auto view =
      mesh.faces
      | std::views::transform(make_triangle)
      | std::views::filter(check_back_face_culling);

  auto materialized = std::vector(view.begin(), view.end());
  sort_by_depth(materialized);

  return materialized;
}

void update(const float dt, Mesh& mesh) {
  constexpr float rotate_speed = 0.0005f;

  mesh.rotation.x += dt * rotate_speed;
  // mesh.rotation.y += dt * rotate_speed;
  // mesh.rotation.z += dt * rotate_speed;
}

void render_scene(graphics::Context& context,
                  const Mesh& mesh,
                  const Vec3 camera_pos) {

  for (const auto triangles = transform_mesh(mesh, camera_pos); const Triangle& t : triangles) {
    const Vec2 a = project(t.a, camera_pos);
    const Vec2 b = project(t.b, camera_pos);
    const Vec2 c = project(t.c, camera_pos);

    draw::filled_triangle(context, a, b, c, 0xFFFFFFFF);
    draw::triangle(context, a, b, c, 0xFF000000);
  }
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

  Mesh test_mesh;
  load_obj_file("./assets/f22.obj", test_mesh);

  bool quit = false;
  auto renderer = graphics::Context(graphics::window::width, graphics::window::height);
  auto frame_limiter = FrameLimiter(60, enable_v_sync);

  while (!quit) {
    constexpr Vec3 camera_pos = {0, 0, -5};

    quit = process_input();
    const float dt = frame_limiter.tick();

    update(dt, test_mesh);

    render_scene(renderer, test_mesh, camera_pos);
    renderer.present(*sdl_renderer, *display_texture);
  }

  destroy(window, sdl_renderer);

  return 0;
}
