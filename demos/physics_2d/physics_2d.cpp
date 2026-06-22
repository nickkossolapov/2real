#include "drawer.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/force.h"
#include "physics/particle.h"
#include "render/color.h"
#include "render/framebuffer.h"

#include <set>

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 960,
      .height = 720,
      .scale = 1,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {
      .sdl_settings = settings,
      .background = render::color::near_black,
  };

  constexpr float pixels_per_meter = 10.0f;

  Drawer drawer(pixels_per_meter);

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  bool is_holding = false;
  int held_particle = 0;
  math::Vec2 pointer;

  std::vector<physics::Particle> particles{};
  std::set<std::pair<int, int>> connections{};

  constexpr int width = 16;
  constexpr int height = 16;
  constexpr float x_pitch = 2.5f;
  constexpr float y_pitch = 2.5f;
  constexpr float y0 = 25.0f;
  constexpr float x0 = 25.0f;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      const math::Vec2 pos{.x = x0 + x * x_pitch, .y = y0 + y * y_pitch};

      // const float mass = (y + 1) == height && (x == 0 || (x + 1) == width) ? 0 : 0.1f;
      // const float mass = (y + 1) == height ? 0 : 0.1f;
      const float mass = x == 0 ? 0 : 0.05f;
      // constexpr float mass = 0.1f;

      particles.emplace_back(physics::Particle(mass, 0.2, pos));
    }
  }

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      const int current = y * height + x;
      const int next_row = (y + 1) * height + x;

      const bool top = y + 1 == width;
      const bool right = x + 1 == height;
      const bool left = x == 0;

      if (!right)
        connections.emplace(current, current + 1);
      if (!top)
        connections.emplace(current, next_row);
      if (!top && !right)
        connections.emplace(current, next_row + 1);
      if (!top && !left)
        connections.emplace(current, next_row - 1);
    }
  }

  auto update = [&particles, &connections](const float dt, const input::InputState& input) {
    constexpr physics::force::SpringParams spring_params{.rest_length = 3, .k = 10, .c = 2};

    for (auto& [a_index, b_index] : connections) {
      auto& a = particles[a_index];
      auto& b = particles[b_index];

      const math::Vec2 force = physics::force::spring(a, b, spring_params);

      a.add_force(force);
      b.add_force(-force);
    }

    for (auto& p : particles) {
      p.add_force(physics::force::gravity(p.mass));
    }

    for (auto& p : particles) {
      p.integrate(dt);
    }

    for (auto& p : particles) {
      // Note: not real physics for now
      constexpr float restitution = 0.9f;

      if (p.position.x - p.radius <= 0) {
        p.position.x = p.radius;
        p.velocity.x *= -restitution;
      } else if (p.position.x + p.radius >= world_width) {
        p.position.x = world_width - p.radius;
        p.velocity.x *= -restitution;
      }

      if (p.position.y - p.radius <= 0) {
        p.position.y = p.radius;
        p.velocity.y *= -restitution;
      } else if (p.position.y + p.radius >= world_height) {
        p.position.y = world_height - p.radius;
        p.velocity.y *= -restitution;
      }
    }
  };

  auto read_input = [&particles, &pointer, &is_holding, &held_particle](const input::InputState& state,
                                                                        const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    if (events.primary == input::Event::Pressed) {
      for (int i = 0; i < particles.size(); ++i) {
        const auto p = particles[i];

        if ((pointer - p.position).length() <= p.radius) {
          is_holding = true;
          held_particle = i;
          break;
        }
      }
    }

    if (events.primary == input::Event::Released) {
      if (is_holding) {
        particles[held_particle].velocity = pointer - particles[held_particle].position;
      }

      is_holding = false;
    }
  };

  auto render = [&particles, &connections, &is_holding, &held_particle, &pointer, &drawer](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, particles[held_particle].position, pointer, render::color::red);
    }

    drawer.filled_circle(fb, pointer, 1, render::color::red);

    for (auto& [a, b] : connections) {
      drawer.line(fb, particles[a].position, particles[b].position, render::color::white);
    }

    for (auto& p : particles) {
      drawer.filled_circle(fb, p.position, p.radius, render::color::white);
    }
  };

  return engine::run(app_config, read_input, update, render);
}
