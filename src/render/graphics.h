#pragma once

#include "../engine/init.h"

#include <vector>
#include <SDL3/SDL.h>

namespace graphics {

class Context {
public:
  Context(int width, int height);
  void draw_pixel(int x, int y, uint32_t color);
  void present(SDL_Renderer& renderer, SDL_Texture& display_texture);

private:
  int width_, height_;
  std::vector<uint32_t> color_buffer_;
};

} // namespace graphics
