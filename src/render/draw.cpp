#include "draw.h"

#include <algorithm>
#include <cmath>

namespace render::draw {

namespace {

void flat_line(Context& context, const int y, const float x1, const float x2, const uint32_t color) {
  const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
  const int right = static_cast<int>(std::floor(std::max(x1, x2)));

  for (int x = left; x <= right; x++) {
    context.draw_pixel(x, y, color);
  }
}

void filled_flat_bottom(Context& context, const std::array<math::Vec2, 3>& vertices, const uint32_t color) {
  auto [top, mid1, mid2] = vertices;

  const float m1 = (mid1.x - top.x) / (mid1.y - top.y);
  const float m2 = (mid2.x - top.x) / (mid2.y - top.y);

  const int y_start = static_cast<int>(std::ceil(top.y));
  const int y_end = static_cast<int>(std::floor(mid1.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - top.y;
    const float x_start = top.x + m1 * dy;
    const float x_end = top.x + m2 * dy;

    flat_line(context, y, x_start, x_end, color);
  }
}

void filled_flat_top(Context& context, const std::array<math::Vec2, 3>& vertices, const uint32_t color) {
  auto [bottom, mid1, mid2] = vertices;

  const float m1 = (mid1.x - bottom.x) / (mid1.y - bottom.y);
  const float m2 = (mid2.x - bottom.x) / (mid2.y - bottom.y);

  const int y_start = static_cast<int>(std::ceil(mid1.y));
  const int y_end = static_cast<int>(std::floor(bottom.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - bottom.y;
    const float x_start = bottom.x + m1 * dy;
    const float x_end = bottom.x + m2 * dy;

    flat_line(context, y, x_start, x_end, color);
  }
}

} // namespace

void rect(Context& context, const math::Vec2& top_left, const int w, const int h, const uint32_t color) {
  const int x = static_cast<int>(top_left.x);
  const int y = static_cast<int>(top_left.y);

  for (int i = x; i < x + w; i++) {
    for (int j = y; j < y + h; j++) {
      context.draw_pixel(i, j, color);
    }
  }
}

// dda line drawing
void line(Context& context, const math::Vec2& v0, const math::Vec2& v1, const uint32_t color) {
  auto [x0, y0] = v0;
  auto [x1, y1] = v1;

  const float delta_x = x1 - x0;
  const float delta_y = y1 - y0;

  const int side_length =
      static_cast<int>(std::abs(delta_x) >= std::abs(delta_y) ? std::abs(delta_x) : std::abs(delta_y));

  if (side_length == 0) {
    context.draw_pixel(std::lround(x0), std::lround(y0), color);
    return;
  }

  const float x_inc = delta_x / side_length;
  const float y_inc = delta_y / side_length;

  float current_x = x0;
  float current_y = y0;

  for (int i = 0; i <= side_length; i++) {
    context.draw_pixel(std::lround(current_x), std::lround(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
}

void filled_triangle(Context& context, std::array<math::Vec2, 3> v, const uint32_t color) {
  std::ranges::sort(v, [](const math::Vec2 l, const math::Vec2 r) { return l.y < r.y; });

  if (std::abs(v[1].y - v[2].y) < 0.01f) {
    filled_flat_bottom(context, {v[0], v[2], v[1]}, color);
  } else if (std::abs(v[0].y - v[1].y) < 0.01f) {
    filled_flat_top(context, {v[2], v[0], v[1]}, color);
  } else {

    const math::Vec2 v_mid = {.x = v[0].x + (v[2].x - v[0].x) * (v[1].y - v[0].y) / (v[2].y - v[0].y), .y = v[1].y};

    filled_flat_bottom(context, {v[0], v[1], v_mid}, color);
    filled_flat_top(context, {v[2], v[1], v_mid}, color);
  }
}

} // namespace render::draw
