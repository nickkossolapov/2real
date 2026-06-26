#pragma once
#include "math/vec2.h"
#include "render/draw.h"
#include "render/framebuffer.h"

#include <algorithm>
#include <array>

/// Class to invert y so that bottom left is 0,0 since this 2D view is physics focused
class Drawer {
public:
  explicit Drawer(const float pixels_per_meter)
      : pixels_per_meter_(pixels_per_meter) {};

  void rect(render::Framebuffer& fb, const math::Vec2& top_left, const int w, const int h, const uint32_t color) const {
    render::draw::rect(fb, to_screen(fb, top_left), w * pixels_per_meter_, h * pixels_per_meter_, color);
  }

  void line(render::Framebuffer& fb, const math::Vec2& v0, const math::Vec2& v1, uint32_t color) const {
    render::draw::line(fb, to_screen(fb, v0), to_screen(fb, v1), color);
  }

  void filled_triangle(render::Framebuffer& fb, const std::array<math::Vec2, 3>& vertices, const uint32_t color) const {
    std::array<render::draw::FlatVertex, 3> scaled;
    std::ranges::transform(vertices, scaled.begin(), [this, &fb](const math::Vec2& v) {
      return render::draw::FlatVertex{to_screen(fb, v)};
    });

    render::draw::filled_triangle(fb, scaled, color);
  }

  void filled_circle(render::Framebuffer& fb,
                     const math::Vec2& center,
                     const float radius,
                     const uint32_t color) const {
    render::draw::filled_circle(fb, to_screen(fb, center), radius * pixels_per_meter_, color);
  }

  void debug_circle(render::Framebuffer& fb,
                    const math::Vec2& center,
                    const float radius,
                    const float angle,
                    const uint32_t color) const {
    const math::Vec2 edge_point = math::Vec2{std::sin(angle), std::cos(angle)} * radius;

    render::draw::circle(fb, to_screen(fb, center), radius * pixels_per_meter_, color);
    render::draw::line(fb, to_screen(fb, center), to_screen(fb, center + edge_point), color);
  }

private:
  math::Vec2 to_screen(const render::Framebuffer& fb, const math::Vec2& world) const {
    return {world.x * pixels_per_meter_, fb.height() - world.y * pixels_per_meter_};
  }

  float pixels_per_meter_ = 0; // pixels per meter
};