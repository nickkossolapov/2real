#pragma once
#include "../math/vec2.h"
#include "context.h"
#include "texture.h"

#include <array>

namespace render::draw {

struct TexturedVertex {
  math::Vec2 position;
  math::Vec2 uv;
};

void rect(Context&, const math::Vec2& top_left, int w, int h, uint32_t color);
void line(Context&, const math::Vec2& v0, const math::Vec2& v1, uint32_t color);
void triangle(Context&, const math::Vec2& v0, const math::Vec2& v1, const math::Vec2& v2, uint32_t color);
void filled_triangle(Context&, std::array<math::Vec2, 3> v, uint32_t color);
void textured_triangle(Context&, std::array<TexturedVertex, 3> vertices, const Texture& texture);

} // namespace render::draw
