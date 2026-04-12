#include "draw.h"

#include <cmath>

using namespace graphics;

namespace draw {

void rect(Context& context, const Vec2& top_left, const int w, const int h, const uint32_t color) {
  const int x = static_cast<int>(top_left.x);
  const int y = static_cast<int>(top_left.y);

  for (int i = x; i < x + w; i++) {
    for (int j = y; j < y + h; j++) {
      context.draw_pixel(i, j, color);
    }
  }
}

// dda line drawing
void line(Context& context, const Vec2& v0, const Vec2& v1, const uint32_t color) {
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

void triangle(Context& context, const Vec2& v0, const Vec2& v1, const Vec2& v2, const uint32_t color) {
  line(context, v0, v1, color);
  line(context, v1, v2, color);
  line(context, v2, v0, color);
};

namespace {

void flat_line(Context& context, const float yf, const float x1, const float x2, const uint32_t color) {
  const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
  const int right = static_cast<int>(std::floor(std::max(x1, x2)));
  const int y = std::lround(yf);

  for (int x = left; x <= right; x++) {
    context.draw_pixel(x, y, color);
  }
}

void filled_flat_bottom(Context& context, const Vec2& top, const Vec2& mid1, const Vec2& mid2, const uint32_t color) {
  const float m1 = (top.x - mid1.x) / (top.y - mid1.y);
  const float m2 = (top.x - mid2.x) / (top.y - mid2.y);

  float x_start = top.x, x_end = top.x;

  for (float y = top.y; y < mid1.y; y++) {
    flat_line(context, y, x_start, x_end, color);

    x_start += m1;
    x_end += m2;
  }
};

void filled_flat_top(Context& context, const Vec2& bottom, const Vec2& mid1, const Vec2& mid2, const uint32_t color) {
  const float m1 = (bottom.x - mid1.x) / (bottom.y - mid1.y);
  const float m2 = (bottom.x - mid2.x) / (bottom.y - mid2.y);

  float x_start = bottom.x, x_end = bottom.x;

  for (float y = bottom.y; y > mid1.y; y--) {
    flat_line(context, y, x_start, x_end, color);

    x_start -= m1;
    x_end -= m2;
  }
};

}

void filled_triangle(Context& context, const Vec2& v0, const Vec2& v1, const Vec2& v2, const uint32_t color) {
  Vec2 v_top = v0, v_mid = v1, v_bottom = v2;

  if (v_top.y > v_mid.y) {
    std::swap(v_top, v_mid);
  }

  if (v_mid.y > v_bottom.y) {
    std::swap(v_mid, v_bottom);
  }

  if (v_top.y > v_mid.y) {
    std::swap(v_top, v_mid);
  }

  if (v_bottom.y - v_top.y < std::numeric_limits<float>::epsilon()) {
    return; // Don't draw triangles with no height
  }

  Vec2 v_mid_computed = {
      .x = v_top.x + (v_bottom.x - v_top.x) * (v_mid.y - v_top.y) / (v_bottom.y - v_top.y),
      .y = v_mid.y
  };

  if (v_mid.x > v_mid_computed.x) {
    std::swap(v_mid, v_mid_computed);
  }

  filled_flat_bottom(context, v_top, v_mid, v_mid_computed, color);
  filled_flat_top(context, v_bottom, v_mid, v_mid_computed, color);
  flat_line(context, v_mid.y, v_mid.x, v_mid_computed.x, color);
};

} // namespace draw
