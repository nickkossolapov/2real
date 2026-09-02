#include "drawer.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "engine/sparse_logger.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/body.h"
#include "physics/world.h"
#include "render/color.h"
#include "render/framebuffer.h"

#include <set>

namespace {

math::Vec2 to_world(const math::Vec2 center, const math::Vec2 local, const float a) {
  // left-handed rotation matrix
  const float x = std::cos(a) * local.x + std::sin(a) * local.y;
  const float y = -std::sin(a) * local.x + std::cos(a) * local.y;

  return center + math::Vec2{x, y};
}

void render_body(const Drawer& drawer, render::Framebuffer& fb, const physics::Body& body, uint32_t color) {
  auto visitor = Overloaded{
      [&](const physics::shape::Circle& c) { drawer.debug_circle(fb, body.position, c.radius, body.rotation, color); },
      [&](const physics::shape::Polygon& p) {
        const int count = p.points.capacity();

        for (int i = 0; i < count; ++i) {
          const int next = (i + 1) % count;

          drawer.line(fb,
                      to_world(body.position, p.points[i], body.rotation),
                      to_world(body.position, p.points[next], body.rotation),
                      color);
        }
      },
  };

  return std::visit(visitor, body.shape());
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 1000,
      .height = 800,
      .scale = 1,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {
      .sdl_settings = settings,
      .background = render::color::near_black,
  };

  constexpr float pixels_per_meter = 20.0f;
  Drawer drawer(pixels_per_meter);
  engine::SparseLogger logger;

  bool is_holding = false;
  int held_particle = 0;
  math::Vec2 pointer;

  physics::World world{};

  world.add_body(physics::Body{0.0f, 0.3f, physics::shape::Circle(1.0f), math::Vec2{.x = 20.0f, .y = 30.0f}});

  constexpr int ball_count = 10;
  for (int i = 1; i < ball_count; ++i) {
    auto next =
        world.add_body(physics::Body{1.0f, 0.3f, physics::shape::Circle(0.5f), math::Vec2{20.0f + 2.0f * i, 30.0f}});
    std::unique_ptr<physics::Body>& prev = world.get_body(i - 1);

    world.add_joint_constraint(physics::JointConstraint(*prev, *next, prev->position));
  }

  auto update = [&world](const float dt, const input::InputState& input) { world.update(dt); };

  auto read_input = [&world, &pointer](const input::InputState& state, const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    if (events.primary == input::Event::Released) {
      world.add_body(physics::Body{1.0f, 0.5f, physics::shape::Circle(1.0f), pointer});
    }

    if (events.secondary == input::Event::Released) {
      world.add_body(physics::Body{1.0f, 0.5f, physics::shape::box(2.0f, 2.0f), pointer});
    }
  };

  auto render = [&world, &is_holding, &held_particle, &pointer, &drawer](render::Framebuffer& fb) {
    const auto& bodies = world.bodies();

    if (is_holding) {
      drawer.line(fb, bodies[held_particle]->position, pointer, render::color::red);
    }

    for (auto& body : bodies) {
      render_body(drawer, fb, *body, render::color::white);
    }
  };

  return engine::run(app_config, read_input, update, render);
}
