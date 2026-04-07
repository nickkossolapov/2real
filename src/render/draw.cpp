#include "draw.h"

#include <cmath>

namespace draw {

void rect(graphics::Context& context, const int x, const int y, const int w, const int h, const uint32_t color) {
  for (int i = x; i < x + w; i++) {
    for (int j = y; j < y + h; j++) {
      context.draw_pixel(i, j, color);
    }
  }
}


// dda line drawing
void line(graphics::Context& context, const int x0, const int y0, const int x1, const int y1, const uint32_t color) {
  const int delta_x = x1 - x0;
  const int delta_y = y1 - y0;

  const int side_length = std::abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

  const float x_inc = delta_x / static_cast<float>(side_length);
  const float y_inc = delta_y / static_cast<float>(side_length);

  float current_x = x0;
  float current_y = y0;

  for (int i = 0; i <= side_length; i++) {
    context.draw_pixel(std::lround(current_x), std::lround(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
}

} // namespace draw
