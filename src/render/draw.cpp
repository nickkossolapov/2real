#include "draw.h"

#include <algorithm>
#include <cmath>

namespace render::draw {

namespace {

struct PixelResult {
  uint32_t color;
  float inv_w;
};

constexpr int kSubpixelBits = 4;
constexpr int32_t kSubpixelScale = 1 << kSubpixelBits;

struct Vec2i {
  int32_t x, y;
};

// The three edge-function values at a pixel. Held at scale S^2 (see edge): never
// rescaled, since only their sign (coverage) and ratio to the area (barycentrics)
// are used, and both are scale-independent.
struct EdgeWeights {
  int32_t w0, w1, w2;

  EdgeWeights operator+(const EdgeWeights& e) const { return {w0 + e.w0, w1 + e.w1, w2 + e.w2}; }

  EdgeWeights& operator+=(const EdgeWeights& e) {
    w0 += e.w0;
    w1 += e.w1;
    w2 += e.w2;

    return *this;
  }

  bool all_non_negative() const { return w0 >= 0 && w1 >= 0 && w2 >= 0; }

  std::array<float, 3> normalized(const double inv_area) const {
    return {static_cast<float>(w0 * inv_area), static_cast<float>(w1 * inv_area), static_cast<float>(w2 * inv_area)};
  }
};

// Integer exact edge function. To avoid truncating a fixed point integer to
int32_t edge(const Vec2i& a, const Vec2i& b, const Vec2i& p) {
  const int64_t e = static_cast<int64_t>(b.x - a.x) * (p.y - a.y) - static_cast<int64_t>(b.y - a.y) * (p.x - a.x);

  return static_cast<int32_t>(e);
}

int32_t top_left_bias(const Vec2i& start, const Vec2i& end) {
  const int32_t dx = end.x - start.x;
  const int32_t dy = end.y - start.y;

  const bool is_top_edge = dy == 0 && dx > 0;
  const bool is_left_edge = dy < 0;

  return is_top_edge || is_left_edge ? 0 : -1;
}

Vec2i to_subpixel(const math::Vec2& v) {
  return {static_cast<int32_t>(std::lround(v.x * kSubpixelScale)),
          static_cast<int32_t>(std::lround(v.y * kSubpixelScale))};
}

int32_t floor_div(const int32_t a, const int32_t b) {
  return a >= 0 ? a / b : -((-a + b - 1) / b);
}

int32_t ceil_div(const int32_t a, const int32_t b) {
  return a >= 0 ? (a + b - 1) / b : -(-a / b);
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

float get_depth(const std::array<float, 3>& weights, const std::array<float, 3>& w) {
  return weights[0] / w[0] + weights[1] / w[1] + weights[2] / w[2];
}

// Pineda rasterizer with exact integer edge functions
template <typename PixelFn> void triangle(Context& context, const std::array<Vec2i, 3>& v, const PixelFn pixel_fn) {
  const int32_t area = edge(v[0], v[1], v[2]);

  if (area <= 0) {
    return;
  }

  const EdgeWeights step_x = {
      (v[1].y - v[2].y) * kSubpixelScale,
      (v[2].y - v[0].y) * kSubpixelScale,
      (v[0].y - v[1].y) * kSubpixelScale,
  };
  const EdgeWeights step_y = {
      (v[2].x - v[1].x) * kSubpixelScale,
      (v[0].x - v[2].x) * kSubpixelScale,
      (v[1].x - v[0].x) * kSubpixelScale,
  };
  const EdgeWeights bias = {
      top_left_bias(v[1], v[2]),
      top_left_bias(v[2], v[0]),
      top_left_bias(v[0], v[1]),
  };

  const int32_t min_x = std::min({v[0].x, v[1].x, v[2].x});
  const int32_t max_x = std::max({v[0].x, v[1].x, v[2].x});
  const int32_t min_y = std::min({v[0].y, v[1].y, v[2].y});
  const int32_t max_y = std::max({v[0].y, v[1].y, v[2].y});

  const int x_start = floor_div(min_x, kSubpixelScale);
  const int x_end = ceil_div(max_x, kSubpixelScale);
  const int y_start = floor_div(min_y, kSubpixelScale);
  const int y_end = ceil_div(max_y, kSubpixelScale);

  constexpr int32_t half = kSubpixelScale / 2;
  const Vec2i p0 = {x_start * kSubpixelScale + half, y_start * kSubpixelScale + half};

  EdgeWeights row = {edge(v[1], v[2], p0), edge(v[2], v[0], p0), edge(v[0], v[1], p0)};

  const double inv_area = 1.0 / static_cast<double>(area);

  for (int y = y_start; y <= y_end; ++y) {
    EdgeWeights current = row;

    for (int x = x_start; x <= x_end; ++x) {
      if ((current + bias).all_non_negative()) {
        const PixelResult res = pixel_fn(current.normalized(inv_area));

        context.draw_pixel(x, y, res.inv_w, res.color);
      }

      current += step_x;
    }

    row += step_y;
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
  auto [x0, y0] = v0;
  auto [x1, y1] = v1;

  const float delta_x = x1 - x0;
  const float delta_y = y1 - y0;

  const float side_length = std::abs(delta_x) >= std::abs(delta_y) ? std::abs(delta_x) : std::abs(delta_y);
  const int steps = static_cast<int>(std::lround(side_length));

  if (steps == 0) {
    context.draw_pixel(std::lround(x0), std::lround(y0), color);
    return;
  }

  const float x_inc = delta_x / side_length;
  const float y_inc = delta_y / side_length;

  float x = x0, y = y0;

  for (int i = 0; i <= steps; ++i) {
    context.draw_pixel(std::lround(x), std::lround(y), color);
    x += x_inc;
    y += y_inc;
  }
}

void filled_triangle(Context& context, const std::array<FlatVertex, 3>& vertices, const uint32_t color) {
  const std::array v = {to_subpixel(vertices[0].pos), to_subpixel(vertices[1].pos), to_subpixel(vertices[2].pos)};
  const std::array w = {vertices[0].w, vertices[1].w, vertices[2].w};

  auto pixel_fn = [&w, color](const std::array<float, 3>& weights) {
    const float depth = get_depth(weights, w);
    const PixelResult res = {color, depth};

    return res;
  };

  triangle(context, v, pixel_fn);
}

void textured_triangle(Context& context, const std::array<TexturedVertex, 3>& vertices, const Texture& texture) {
  const std::array v = {to_subpixel(vertices[0].pos), to_subpixel(vertices[1].pos), to_subpixel(vertices[2].pos)};

  auto pixel_fn = [&vertices, &texture](const std::array<float, 3>& weights) {
    return get_texel(weights, vertices, texture);
  };

  triangle(context, v, pixel_fn);
}

} // namespace render::draw
