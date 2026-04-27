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
  auto [top, mid, bottom] = vertices;

  const float m1 = (mid.x - top.x) / (mid.y - top.y);
  const float m2 = (bottom.x - top.x) / (bottom.y - top.y);

  const int y_start = static_cast<int>(std::ceil(top.y));
  const int y_end = static_cast<int>(std::floor(mid.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - top.y;
    const float x_start = top.x + m1 * dy;
    const float x_end = top.x + m2 * dy;

    flat_line(context, y, x_start, x_end, color);
  }
}

void filled_flat_top(Context& context, const std::array<math::Vec2, 3>& vertices, const uint32_t color) {
  auto [top, mid, bottom] = vertices;

  const float m1 = (mid.x - bottom.x) / (mid.y - bottom.y);
  const float m2 = (top.x - bottom.x) / (top.y - bottom.y);

  const int y_start = static_cast<int>(std::ceil(mid.y));
  const int y_end = static_cast<int>(std::floor(bottom.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - bottom.y;
    const float x_start = bottom.x + m1 * dy;
    const float x_end = bottom.x + m2 * dy;

    flat_line(context, y, x_start, x_end, color);
  }
}

std::array<float, 3> get_barycentric_weights(const std::array<TexturedVertex, 3>& v, const math::Vec2& p) {
  std::array<float, 3> weights;

  const float total_area = math::cross(v[1].pos - v[0].pos, v[2].pos - v[0].pos);

  for (int i = 0; i < 3; i++) {
    weights[i] = cross(v[(i + 1) % 3].pos - p, v[(i + 2) % 3].pos - p) / total_area;
  }

  return weights;
}

uint32_t get_texel(const std::array<float, 3>& weights, const std::array<math::Vec2, 3>& uvs, const Texture& texture) {
  const float u = weights[0] * uvs[0].x + weights[1] * uvs[1].x + weights[2] * uvs[2].x;
  const float v = weights[0] * uvs[0].y + weights[1] * uvs[1].y + weights[2] * uvs[2].y;

  const int tex_x = std::clamp(static_cast<int>(u * (texture.width - 1)), 0, texture.width - 1);
  const int tex_y = std::clamp(static_cast<int>(v * (texture.height - 1)), 0, texture.height - 1);

  return texture.data[tex_y * texture.width + tex_x];
}

void textured_flat_bottom(Context& context, const std::array<TexturedVertex, 3>& tv, const Texture& texture) {
  auto [top, mid, bottom] = tv;

  const float m1 = (mid.pos.x - top.pos.x) / (mid.pos.y - top.pos.y);
  const float m2 = (bottom.pos.x - top.pos.x) / (bottom.pos.y - top.pos.y);

  const int y_start = static_cast<int>(std::ceil(top.pos.y));
  const int y_end = static_cast<int>(std::floor(mid.pos.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - top.pos.y;
    const float x1 = top.pos.x + m1 * dy;
    const float x2 = top.pos.x + m2 * dy;

    const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
    const int right = static_cast<int>(std::floor(std::max(x1, x2)));

    for (int x = left; x <= right; x++) {
      math::Vec2 p = {static_cast<float>(x), static_cast<float>(y)};
      const auto weights = get_barycentric_weights(tv, p);
      const uint32_t texel = get_texel(weights, {tv[0].uv, tv[1].uv, tv[2].uv}, texture);

      context.draw_pixel(x, y, texel);
    }
  }
}

void textured_flat_top(Context& context, const std::array<TexturedVertex, 3>& tv, const Texture& texture) {
  auto [top, mid, bottom] = tv;

  const float m1 = (mid.pos.x - bottom.pos.x) / (mid.pos.y - bottom.pos.y);
  const float m2 = (top.pos.x - bottom.pos.x) / (top.pos.y - bottom.pos.y);

  const int y_start = static_cast<int>(std::ceil(mid.pos.y));
  const int y_end = static_cast<int>(std::floor(bottom.pos.y));

  for (int y = y_start; y <= y_end; y++) {
    const float dy = static_cast<float>(y) - bottom.pos.y;
    const float x1 = bottom.pos.x + m1 * dy;
    const float x2 = bottom.pos.x + m2 * dy;

    const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
    const int right = static_cast<int>(std::floor(std::max(x1, x2)));

    for (int x = left; x <= right; x++) {
      math::Vec2 p = {static_cast<float>(x), static_cast<float>(y)};
      const auto weights = get_barycentric_weights(tv, p);
      const uint32_t texel = get_texel(weights, {tv[0].uv, tv[1].uv, tv[2].uv}, texture);

      context.draw_pixel(x, y, texel);
    }
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

    filled_flat_bottom(context, v, color);
    filled_flat_top(context, v, color);
  }
}

void textured_triangle(Context& context, std::array<TexturedVertex, 3> tv, const Texture& texture) {
  std::ranges::sort(tv, [](const auto& l, const auto& r) { return l.pos.y < r.pos.y; });

  if (std::abs(tv[1].pos.y - tv[2].pos.y) < 0.01f) {
    textured_flat_bottom(context, tv, texture);
  } else if (std::abs(tv[0].pos.y - tv[1].pos.y) < 0.01f) {
    textured_flat_top(context, tv, texture);
  } else {

    textured_flat_bottom(context, tv, texture);
    textured_flat_top(context, tv, texture);
  }
};

} // namespace render::draw
