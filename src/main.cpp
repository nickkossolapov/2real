#include "engine/timing.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/draw.h"
#include "render/graphics.h"
#include "render/mesh.h"
#include "render/obj_loader.h"
#include "render/triangle.h"

#include <ranges>
#include <vector>
#include <SDL3/SDL.h>

struct Mesh;

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

std::vector<Triangle> make_triangles(const Mesh& mesh) {
  auto view =
      mesh.faces
      | std::views::transform([mesh](const Face face) {
        return Triangle(
            vec3::rotate(mesh.vertices[face.a], mesh.rotation),
            vec3::rotate(mesh.vertices[face.b], mesh.rotation),
            vec3::rotate(mesh.vertices[face.c], mesh.rotation));
      });

  return std::vector(view.begin(), view.end());
}

void update(const float dt, Mesh& mesh) {
  constexpr float rotate_speed = 0.0005f;

  mesh.rotation.x += dt * rotate_speed;
  mesh.rotation.y += dt * rotate_speed;
  mesh.rotation.z += dt * rotate_speed;
}

void render_scene(graphics::Context& context,
                  const Mesh& mesh,
                  const Vec3 camera_pos) {
  auto triangles = make_triangles(mesh);

  for (const Triangle& t : triangles) {
    const Vec2 a = project(t.a, camera_pos);
    const Vec2 b = project(t.b, camera_pos);
    const Vec2 c = project(t.c, camera_pos);

    draw::line(context, a.x, a.y, b.x, b.y, 0xFFFFFF00);
    draw::line(context, b.x, b.y, c.x, c.y, 0xFFFFFF00);
    draw::line(context, c.x, c.y, a.x, a.y, 0xFFFFFF00);
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
