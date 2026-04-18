#include "engine/timing.h"
#include "math/mat4.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/draw.h"
#include "render/graphics.h"
#include "render/mesh.h"
#include "render/obj_loader.h"
#include "render/triangle.h"

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

math::Vec2 project(const math::Vec3& p, const math::Vec3 camera_pos) {
  constexpr int fov_factor = 640;

  return {
      fov_factor * (p.x - camera_pos.x) / (p.z - camera_pos.z) + graphics::window::width / 2,
      fov_factor * (p.y - camera_pos.y) / (p.z - camera_pos.z) + graphics::window::height / 2
  };
}

bool is_front_facing(const render::Triangle& triangle, const math::Vec3& camera_pos) {
  const math::Vec3 normal = math::cross(triangle.b - triangle.a, triangle.c - triangle.a);
  const math::Vec3 to_camera = camera_pos - triangle.a;

  return math::dot(normal, to_camera) > 0;
}

void sort_by_depth(std::vector<render::Triangle>& triangles) {
  auto comparer = [](const render::Triangle& l, const render::Triangle& r) {
    const float z_l = l.a.z + l.b.z + l.c.z;
    const float z_r = r.a.z + r.b.z + r.c.z;

    return z_l > z_r;
  };

  std::ranges::sort(triangles.begin(), triangles.end(), comparer);
}

std::vector<render::Triangle> transform_entity(const render::Entity& entity, const math::Vec3& camera_pos) {
  math::Mat4 transform_mat = math::mat4::rotation(entity.transform.rotation);

  auto make_triangle = [&entity, &transform_mat](const render::Face& face) {
    const math::Vec3 a = transform_mat.transform_position(entity.mesh->vertices[face.a]).to_vec3();
    const math::Vec3 b = transform_mat.transform_position(entity.mesh->vertices[face.b]).to_vec3();
    const math::Vec3 c = transform_mat.transform_position(entity.mesh->vertices[face.c]).to_vec3();

    return render::Triangle(a, b, c);
  };
  auto check_back_face_culling = [&camera_pos](const render::Triangle& t) { return is_front_facing(t, camera_pos); };

  auto view =
      entity.mesh->faces
      | std::views::transform(make_triangle)
      | std::views::filter(check_back_face_culling);

  auto materialized = std::vector(view.begin(), view.end());
  sort_by_depth(materialized);

  return materialized;
}

void update(const float dt, render::Entity& entity) {
  entity.transform.rotation += dt * 0.0005f;
  entity.transform.scale += dt * 0.0005f;
  entity.transform.position += dt * 0.0005f;
}

void render_scene(graphics::Context& context,
                  const render::Entity& entity,
                  const math::Vec3 camera_pos) {

  for (const auto triangles = transform_entity(entity, camera_pos); const render::Triangle& t : triangles) {
    const math::Vec2 a = project(t.a, camera_pos);
    const math::Vec2 b = project(t.b, camera_pos);
    const math::Vec2 c = project(t.c, camera_pos);

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

  const auto test_mesh = std::make_shared<render::Mesh>();
  load_obj_file("./assets/f22.obj", *test_mesh);

  render::Entity test_entity = {
      .mesh = test_mesh
  };

  bool quit = false;
  auto renderer = graphics::Context(graphics::window::width, graphics::window::height);
  auto frame_limiter = FrameLimiter(60, enable_v_sync);

  while (!quit) {
    constexpr math::Vec3 camera_pos = {0, 0, -5};

    quit = process_input();
    const float dt = frame_limiter.tick();

    update(dt, test_entity);

    render_scene(renderer, test_entity, camera_pos);
    renderer.present(*sdl_renderer, *display_texture);
  }

  destroy(window, sdl_renderer);

  return 0;
}
