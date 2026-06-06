#include "draw.h"

#include "math/fixed.h"
#include "math/vec2_fixed.h"

#include <algorithm>
#include <cmath>

namespace render::draw {

namespace {

using math::Fixed;
using math::Vec2Fixed;

struct PixelResult {
  uint32_t color;
  float inv_w;
};

template <typename Vertex>
std::array<float, 3> get_barycentric_weights(const std::array<Vertex, 3>& v, const math::Vec2& p) {
  std::array<float, 3> weights;

  const float total_area = math::cross(v[1].pos - v[0].pos, v[2].pos - v[0].pos);

  for (int i = 0; i < 3; i++) {
    weights[i] = cross(v[(i + 1) % 3].pos - p, v[(i + 2) % 3].pos - p) / total_area;
  }

  return weights;
}

PixelResult get_texel(const std::array<float, 3>& weights,
                      const std::array<TexturedVertex, 3>& tvs,
                      const Texture& texture) {
  float u = 0.0f;
  float v = 0.0f;
  float inv_w = 0.0f;

  for (int i = 0; i < 3; i++) {
    u += weights[i] * tvs[i].uv.x / tvs[i].w;
    v += weights[i] * tvs[i].uv.y / tvs[i].w;
    inv_w += weights[i] / tvs[i].w;
  }

  u /= inv_w;
  v /= inv_w;

  const int tex_x = std::clamp(static_cast<int>(u * (texture.width - 1)), 0, texture.width - 1);
  const int tex_y = std::clamp(static_cast<int>(v * (texture.height - 1)), 0, texture.height - 1);

  return {texture.data[tex_y * texture.width + tex_x], inv_w};
}

float get_depth(const std::array<float, 3>& weights, const std::array<FlatVertex, 3>& vs) {
  float inv_w = 0.0f;

  for (int i = 0; i < 3; i++) {
    inv_w += weights[i] / vs[i].w;
  }

  return inv_w;
}

template <typename PixelFn, typename Vertex>
void flat_bottom(Context& context, const std::array<Vertex, 3>& vertices, const PixelFn pixel_fn) {
  auto [top, mid, bottom] = vertices;

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
      const auto weights = get_barycentric_weights(vertices, p);
      const auto [color, depth] = pixel_fn(weights);

      context.draw_pixel(x, y, depth, color);
    }
  }
}

template <typename PixelFn, typename Vertex>
void flat_top(Context& context, const std::array<Vertex, 3>& vertices, const PixelFn pixel_fn) {
  auto [top, mid, bottom] = vertices;

  const float m1 = (mid.pos.x - bottom.pos.x) / (mid.pos.y - bottom.pos.y);
  const float m2 = (top.pos.x - bottom.pos.x) / (top.pos.y - bottom.pos.y);

  const int y_start = static_cast<int>(std::ceil(mid.pos.y));
  const int y_end = static_cast<int>(std::floor(bottom.pos.y));

  for (int y = y_start; y <= y_end; ++y) {
    const float dy = static_cast<float>(y) - bottom.pos.y;
    const float x1 = bottom.pos.x + m1 * dy;
    const float x2 = bottom.pos.x + m2 * dy;

    const int left = static_cast<int>(std::ceil(std::min(x1, x2)));
    const int right = static_cast<int>(std::floor(std::max(x1, x2)));

    for (int x = left; x <= right; ++x) {
      math::Vec2 p = {static_cast<float>(x), static_cast<float>(y)};
      const auto weights = get_barycentric_weights(vertices, p);
      const auto [color, depth] = pixel_fn(weights);

      context.draw_pixel(x, y, depth, color);
    }
  }
}

} // namespace

void rect(Context& context, const math::Vec2& top_left, const int w, const int h, const uint32_t color) {
  const int x = static_cast<int>(top_left.x);
  const int y = static_cast<int>(top_left.y);

  for (int i = x; i < x + w; ++i) {
    for (int j = y; j < y + h; ++j) {
      context.draw_pixel(i, j, color);
    }
  }
}

// dda line drawing
void line(Context& context, const math::Vec2& v0, const math::Vec2& v1, const uint32_t color) {
  auto [x0, y0] = Vec2Fixed{v0};
  auto [x1, y1] = Vec2Fixed{v1};

  const Fixed delta_x = x1 - x0;
  const Fixed delta_y = y1 - y0;

  const Fixed side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

  if (side_length.is_zero()) {
    context.draw_pixel(x0.to_int(), y0.to_int(), color);

    return;
  }

  const Fixed x_inc = delta_x / side_length;
  const Fixed y_inc = delta_y / side_length;

  Fixed x = x0;
  Fixed y = y0;

  const int end = side_length.to_int();

  for (int i = 0; i <= end; ++i) {
    context.draw_pixel(x.to_int(), y.to_int(), color);

    x += x_inc;
    y += y_inc;
  }
}

void filled_triangle(Context& context, std::array<FlatVertex, 3> vertices, const uint32_t color) {
  std::ranges::sort(vertices, [](const FlatVertex l, const FlatVertex r) { return l.pos.y < r.pos.y; });

  auto pixel_fn = [&vertices, color](const std::array<float, 3>& weights) {
    const float depth = get_depth(weights, vertices);
    const PixelResult res = {color, depth};

    return res;
  };

  flat_bottom(context, vertices, pixel_fn);
  flat_top(context, vertices, pixel_fn);
}

void textured_triangle(Context& context, std::array<TexturedVertex, 3> vertices, const Texture& texture) {
  std::ranges::sort(vertices, [](const auto& l, const auto& r) { return l.pos.y < r.pos.y; });

  auto pixel_fn = [&vertices, &texture](const std::array<float, 3>& weights) {
    return get_texel(weights, vertices, texture);
  };

  flat_bottom(context, vertices, pixel_fn);
  flat_top(context, vertices, pixel_fn);
}

} // namespace render::draw
