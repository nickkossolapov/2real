#include "draw.h"

#include <cmath>

using namespace graphics;

namespace draw {

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

  const int side_length = static_cast<int>(std::abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y));

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

void triangle(Context& context,
              const math::Vec2& v0,
              const math::Vec2& v1,
              const math::Vec2& v2,
              const uint32_t color) {
  line(context, v0, v1, color);
  line(context, v1, v2, color);
  line(context, v2, v0, color);
};

namespace {

void flat_line(Context& context, const int y, const float x1, const float x2, const uint32_t color) {
  const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
  const int right = static_cast<int>(std::floor(std::max(x1, x2)));

  for (int x = left; x <= right; x++) {
    context.draw_pixel(x, y, color);
  }
}

void filled_flat_bottom(Context& context,
                        const math::Vec2& top,
                        const math::Vec2& mid1,
                        const math::Vec2& mid2,
                        const uint32_t color) {
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

void filled_flat_top(Context& context,
                     const math::Vec2& bottom,
                     const math::Vec2& mid1,
                     const math::Vec2& mid2,
                     const uint32_t color) {
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

}

void filled_triangle(Context& context,
                     const math::Vec2& v0,
                     const math::Vec2& v1,
                     const math::Vec2& v2,
                     const uint32_t color) {
  math::Vec2 v_top = v0, v_mid = v1, v_bottom = v2;

  if (v_top.y > v_mid.y) {
    std::swap(v_top, v_mid);
  }

  if (v_mid.y > v_bottom.y) {
    std::swap(v_mid, v_bottom);
  }

  if (v_top.y > v_mid.y) {
    std::swap(v_top, v_mid);
  }

  if (std::abs(v_mid.y - v_bottom.y) < 0.01f) {
    filled_flat_bottom(context, v_top, v_bottom, v_mid, color);
  } else if (std::abs(v_top.y - v_mid.y) < 0.01f) {
    filled_flat_top(context, v_bottom, v_top, v_mid, color);
  } else {

    const math::Vec2 v_mid_computed = {
        .x = v_top.x + (v_bottom.x - v_top.x) * (v_mid.y - v_top.y) / (v_bottom.y - v_top.y),
        .y = v_mid.y
    };

    filled_flat_bottom(context, v_top, v_mid, v_mid_computed, color);
    filled_flat_top(context, v_bottom, v_mid, v_mid_computed, color);
  };
}


} // namespace draw
