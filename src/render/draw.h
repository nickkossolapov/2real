#pragma once
#include "framebuffer.h"
#include "math/vec2.h"
#include "texture.h"

#include <array>

namespace render::draw {

struct FlatVertex {
  math::Vec2 pos;
  float z;
  float w;
};

struct TexturedVertex {
  math::Vec2 pos;
  math::Vec2 uv;
  float z;
  float w;
};

void rect(Framebuffer&, const math::Vec2& top_left, int w, int h, uint32_t color);
void line(Framebuffer&, const math::Vec2& v0, const math::Vec2& v1, uint32_t color);
void filled_triangle(Framebuffer&, const std::array<FlatVertex, 3>& vertices, uint32_t color);
void textured_triangle(Framebuffer&, const std::array<TexturedVertex, 3>& vertices, const Texture& texture);
void filled_circle(Framebuffer&, const math::Vec2& center, float radius, uint32_t color);

} // namespace render::draw
