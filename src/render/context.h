#pragma once

#include <SDL3/SDL.h>
#include <vector>

namespace render {

class Context {
public:
  Context(int width, int height, uint32_t background_color);
  void draw_pixel(int x, int y, uint32_t color);
  void draw_pixel(int x, int y, float w_inv, uint32_t color);
  void present(SDL_Renderer& renderer, SDL_Texture& display_texture);

private:
  int width_, height_;
  uint32_t background_color_;
  std::vector<uint32_t> color_buffer_;
  std::vector<float> depth_buffer_; // W-buffer for now
};

} // namespace render
