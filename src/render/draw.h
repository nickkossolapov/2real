#pragma once
#include "graphics.h"

namespace draw {

void rect(graphics::Context&, int x, int y, int w, int h, uint32_t color);
void line(graphics::Context&, int x0, int y0, int x1, int y1, uint32_t color);

} // namespace draw
