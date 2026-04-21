#pragma once

#include <SDL3/SDL.h>
#include <vector>

namespace render {

class Context {
public:
  Context(int width, int height);
  void draw_pixel(int x, int y, uint32_t color);
  void present(SDL_Renderer& renderer, SDL_Texture& display_texture);

private:
  int width_, height_;
  std::vector<uint32_t> color_buffer_;
};

} // namespace render
