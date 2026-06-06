#include "draw.h"

#include "math/fixed.h"
#include "math/rect.h"
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

struct EdgeWeights {
  Fixed w0, w1, w2;

  EdgeWeights operator+(const EdgeWeights& w) const { return {w0 + w.w0, w1 + w.w1, w2 + w.w2}; }

  EdgeWeights& operator+=(const EdgeWeights& w) {
    w0 += w.w0;
    w1 += w.w1;
    w2 += w.w2;

    return *this;
  }

  EdgeWeights operator/(const Fixed& f) const { return {w0 / f, w1 / f, w2 / f}; }

  bool all_positive() const { return !w0.is_negative() && !w1.is_negative() && !w2.is_negative(); }
};

// PixelResult get_texel(const std::array<float, 3>& weights,
//                       const std::array<TexturedVertex, 3>& tvs,
//                       const Texture& texture) {
//   float u = 0.0f;
//   float v = 0.0f;
//   float inv_w = 0.0f;
//
//   for (int i = 0; i < 3; i++) {
//     u += weights[i] * tvs[i].uv.x / tvs[i].w;
//     v += weights[i] * tvs[i].uv.y / tvs[i].w;
//     inv_w += weights[i] / tvs[i].w;
//   }
//
//   u /= inv_w;
//   v /= inv_w;
//
//   const int tex_x = std::clamp(static_cast<int>(u * (texture.width - 1)), 0, texture.width - 1);
//   const int tex_y = std::clamp(static_cast<int>(v * (texture.height - 1)), 0, texture.height - 1);
//
//   return {texture.data[tex_y * texture.width + tex_x], inv_w};
// }

float get_depth(const EdgeWeights& weights, const std::array<float, 3>& w) {
  return weights.w0.to_float() / w[0] + weights.w1.to_float() / w[1] + weights.w2.to_float() / w[2];
}

Fixed edge_cross(const Vec2Fixed& v1, const Vec2Fixed& v2, const Vec2Fixed& p) {
  return math::cross(v2 - v1, p - v1);
}

Fixed get_top_left_edge_bias(const Vec2Fixed& start, const Vec2Fixed& end) {
  const auto [dx, dy] = end - start;

  const bool is_top_edge = dy.is_zero() && dx.is_positive();
  const bool is_left_edge = dy.is_negative();

  const bool is_top_left_edge = is_top_edge || is_left_edge;

  return is_top_left_edge ? Fixed{0} : -Fixed::epsilon();
}

// Pineda algorithm for rasterizing triangles
template <typename PixelFn> void triangle(Context& context, const std::array<Vec2Fixed, 3>& v, const PixelFn pixel_fn) {
  const Fixed area = edge_cross(v[0], v[1], v[2]);

  if (area.is_zero()) {
    context.draw_pixel(v[0].x.floor() + 1, v[0].y.floor() + 1, 0xFFFF0000);
  }

  if (!area.is_positive()) {
    return;
  }

  const EdgeWeights delta_x = {v[1].y - v[2].y, v[2].y - v[0].y, v[0].y - v[1].y};
  const EdgeWeights delta_y = {v[2].x - v[1].x, v[0].x - v[2].x, v[1].x - v[0].x};
  const EdgeWeights biases = {get_top_left_edge_bias(v[1], v[2]),
                              get_top_left_edge_bias(v[2], v[0]),
                              get_top_left_edge_bias(v[0], v[1])};

  const auto [x_min, x_max, y_min, y_max] = math::bounding_box(v);
  const int x_start = floor(x_min), x_end = ceil(x_max), y_start = floor(y_min), y_end = ceil(y_max);

  const auto half = Fixed{0.5f};
  const auto p0 = Vec2Fixed{Fixed{x_start} + half, Fixed{y_start} + half};

  EdgeWeights row = {
      edge_cross(v[1], v[2], p0),
      edge_cross(v[2], v[0], p0),
      edge_cross(v[0], v[1], p0),
  };

  for (int y = y_start; y < y_end; ++y) {
    EdgeWeights current = row;

    for (int x = x_start; x < x_end; ++x) {
      bool base = (current + biases).all_positive();
      bool unbiased = current.all_positive();

      if (!base && unbiased) {
        context.draw_pixel(x + 1, y + 1, 1.0f, 0xFFFF0000);
      }

      if ((current + biases).all_positive()) {
        const auto weights = current / area;
        const PixelResult res = pixel_fn(weights);

        context.draw_pixel(x, y, res.inv_w, res.color);
      }

      current += delta_x;
    }

    row += delta_y;
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

// DDA line drawing
void line(Context& context, const math::Vec2& v0, const math::Vec2& v1, const uint32_t color) {
  auto [x0, y0] = Vec2Fixed{v0};
  auto [x1, y1] = Vec2Fixed{v1};

  const Fixed delta_x = x1 - x0;
  const Fixed delta_y = y1 - y0;

  const Fixed side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

  if (side_length.is_zero()) {
    context.draw_pixel(x0.ceil(), y0.ceil(), color);

    return;
  }

  const Fixed x_inc = delta_x / side_length;
  const Fixed y_inc = delta_y / side_length;

  Fixed x = x0;
  Fixed y = y0;

  const int end = side_length.round();

  for (int i = 0; i <= end; ++i) {
    context.draw_pixel(x.round(), y.round(), color);

    x += x_inc;
    y += y_inc;
  }
}

void filled_triangle(Context& context, const std::array<FlatVertex, 3>& v, const uint32_t color) {
  const std::array v_fixed = {Vec2Fixed{v[0].pos}, Vec2Fixed{v[1].pos}, Vec2Fixed{v[2].pos}};
  const std::array w_fixed{v[0].w, v[1].w, v[2].w};

  auto pixel_fn = [&w_fixed, color](const EdgeWeights& weights) {
    const float depth = get_depth(weights, w_fixed);
    const PixelResult res = {color, depth};

    return res;
  };

  triangle(context, v_fixed, pixel_fn);
}

void textured_triangle(Context& context, std::array<TexturedVertex, 3> vertices, const Texture& texture) {
  // std::ranges::sort(vertices, [](const auto& l, const auto& r) { return l.pos.y < r.pos.y; });
  //
  // auto pixel_fn = [&vertices, &texture](const std::array<float, 3>& weights) {
  //   return get_texel(weights, vertices, texture);
  // };
  //
  // flat_bottom(context, vertices, pixel_fn);
  // flat_top(context, vertices, pixel_fn);
}

} // namespace render::draw
