#pragma once
#include "graphics.h"
#include "../math/vec2.h"

namespace draw {

void rect(graphics::Context&, const math::Vec2& top_left, int w, int h, uint32_t color);
void line(graphics::Context&, const math::Vec2& v0, const math::Vec2& v1, uint32_t color);
void triangle(graphics::Context&, const math::Vec2& v0, const math::Vec2& v1, const math::Vec2& v2, uint32_t color);
void filled_triangle(graphics::Context&, const math::Vec2& v0, const math::Vec2& v1, const math::Vec2& v2, uint32_t color);

} // namespace draw
