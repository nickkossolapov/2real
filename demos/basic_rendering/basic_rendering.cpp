#include "engine/fps_logger.h"
#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/fixed.h"
#include "math/rect.h"
#include "math/vec2_fixed.h"
#include "math/vec3.h"
#include "render/context.h"
#include "render/obj_loader.h"
#include "render/pipeline.h"
#include "render/texture_loader.h"
#include "render/viewport.h"
#include "scene/camera.h"
#include "scene/camera_controller.h"
#include "scene/mesh.h"

#include <algorithm>
#include <format>

namespace {

void update(const float dt, const input::State& input, scene::Camera& camera) {
  update_fps_camera(camera, input);
}

std::optional<scene::Entity> create_entity(const std::string& name) {
  const auto mesh = render::load_obj_file(std::format("./assets/{0}.obj", name));
  const auto texture = render::load_texture_file(std::format("./assets/{0}.png", name));

  if (!mesh.has_value()) {
    SDL_Log("Failed to load mesh for %s", name.data());

    return {};
  }

  scene::Entity entity = {.mesh = std::make_shared<scene::Mesh>(std::move(*mesh))};

  if (!texture.has_value()) {
    SDL_Log("Failed to load texture for %s", name.data());

    return {};
  }

  entity.texture = std::make_shared<render::Texture>(texture.value());

  return entity;
}

math::Fixed edge_cross(const math::Vec2Fixed& v1, const math::Vec2Fixed& v2, const math::Vec2Fixed& p) {
  return math::cross(v2 - v1, p - v1);
}

bool is_top_left_edge(const math::Vec2Fixed& start, const math::Vec2Fixed& end) {
  const auto [dx, dy] = end - start;

  const bool is_top_edge = dy.is_zero() && dx.is_positive();
  const bool is_left_edge = dy.is_negative();

  return is_top_edge || is_left_edge;
}

math::Fixed get_top_left_edge_bias(const math::Vec2Fixed& start, const math::Vec2Fixed& end) {
  return is_top_left_edge(start, end) ? math::Fixed{0} : -math::Fixed::epsilon();
}

uint32_t to_argb(const int32_t r, const int32_t g, const int32_t b) {
  constexpr uint32_t a = 0xFF;

  return a << 24 | r << 16 | g << 8 | b;
}

math::Vec2 rotate(const math::Vec2& v, const math::Vec2& center, float a) {
  const math::Vec2 c = v - center;
  const auto rot = math::Vec2{c.x * std::cos(a) - c.y * std::sin(a), c.x * std::sin(a) + c.y * std::cos(a)};

  return rot + center;
}

void triangle_fill(render::Context& context, const std::array<math::Vec2, 3>& v) {
  using math::Fixed;
  using math::Vec2Fixed;

  const Vec2Fixed v0{v[0]};
  const Vec2Fixed v1{v[1]};
  const Vec2Fixed v2{v[2]};

  const Fixed area = edge_cross(v0, v1, v2);

  if (!area.is_positive()) {
    return;
  }

  const auto delta_w0 = Vec2Fixed{v1.y - v2.y, v2.x - v1.x};
  const auto delta_w1 = Vec2Fixed{v2.y - v0.y, v0.x - v2.x};
  const auto delta_w2 = Vec2Fixed{v0.y - v1.y, v1.x - v0.x};

  const Fixed bias0 = get_top_left_edge_bias(v1, v2);
  const Fixed bias1 = get_top_left_edge_bias(v2, v0);
  const Fixed bias2 = get_top_left_edge_bias(v0, v1);

  const auto [x_min, x_max, y_min, y_max] = math::bounding_box(v[0], v[1], v[2]);

  const auto half = Fixed{0.5f};
  const auto p0 = Vec2Fixed{Fixed{x_min} + half, Fixed{y_min} + half};

  Fixed w0_row = edge_cross(v1, v2, p0);
  Fixed w1_row = edge_cross(v2, v0, p0);
  Fixed w2_row = edge_cross(v0, v1, p0);

  for (int y = y_min; y < y_max; ++y) {
    Fixed w0 = w0_row;
    Fixed w1 = w1_row;
    Fixed w2 = w2_row;

    for (int x = x_min; x < x_max; ++x) {
      if (w0 + bias0 >= Fixed{0} && w1 + bias1 >= Fixed{0} && w2 + bias2 >= Fixed{0}) {
        const Fixed alpha = w0 / area;
        const Fixed beta = w1 / area;
        const Fixed gamma = w2 / area;

        constexpr auto white = Fixed{0xFF};

        const uint32_t blended_color =
            to_argb((alpha * white).to_int(), (beta * white).to_int(), (gamma * white).to_int());

        context.draw_pixel(x, y, blended_color);
      }

      w0 += delta_w0.x;
      w1 += delta_w1.x;
      w2 += delta_w2.x;
    }

    w0_row += delta_w0.y;
    w1_row += delta_w1.y;
    w2_row += delta_w2.y;
  }
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr bool enable_v_sync = true;

  engine::SdlContext sdl;
  if (auto err = sdl.init(enable_v_sync); err != engine::InitError::None) {
    return static_cast<int>(err);
  }

  constexpr float fov = math::deg_to_rad(60.0f);
  constexpr float aspect = static_cast<float>(engine::window::width) / static_cast<float>(engine::window::height);

  scene::Camera camera = {.fov = fov, .aspect_ratio = aspect, .z_near = 0.1f, .z_far = 100.0f, .position = {0, 0, 0}};

  const auto light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f});

  bool quit = false;
  constexpr render::Viewport viewport = {engine::window::width, engine::window::height};

  constexpr uint32_t blue = 0xFF87CEEB;

  auto f22 = create_entity("f22").value();
  auto f117 = create_entity("f117").value();
  auto efa = create_entity("efa").value();
  auto runway = create_entity("runway").value();

  camera.position.x = 1;
  camera.position.y = 2;
  camera.position.z = 6;
  camera.rotation.y = math::deg_to_rad(170);
  camera.rotation.x = math::deg_to_rad(-20);

  f22.transform.position.z = 0;
  f22.transform.rotation.y = math::deg_to_rad(90);

  f117.transform.position.z = 1;
  f117.transform.position.x = 2;
  f117.transform.rotation.y = math::deg_to_rad(120);

  efa.transform.position.z = 1;
  efa.transform.position.x = -2;
  efa.transform.rotation.y = math::deg_to_rad(60);

  runway.transform.position.y = -0.35;

  std::vector entities{f22, f117, efa, runway};

  std::vector<math::Vec2> vertices = {
      {40, 40},
      {80, 40},
      {40, 80},
      {90, 90},
      {75, 20},
  };

  auto renderer = render::Context(viewport.width, viewport.height, blue);
  auto frame_limiter = engine::FrameLimiter(60, enable_v_sync);

  input::State input_state;

  engine::FpsLogger fps_logger;

  while (!quit) {
    const float dt = frame_limiter.tick() / 1000.0f; // Convert to seconds

    quit = input::process_input(dt, sdl.gamepad(), input_state);
    update(dt, input_state, camera);

    render::pipeline::render_entity(renderer, viewport, f22, camera, light, render::RenderMode::Wireframe);

    renderer.present(sdl.renderer(), sdl.display_texture());

    fps_logger.tick();
  }

  return 0;
}
