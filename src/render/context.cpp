#include "context.h"

#include <algorithm>

namespace render {

Context::Context(const int width, const int height)
    : width_(width),
      height_(height),
      color_buffer_(height * width),
      depth_buffer_(height * width) {}

void Context::draw_pixel(const int x, const int y, const uint32_t color) {
  if (x >= 0 && x < width_ && y >= 0 && y < height_) {
    color_buffer_[y * width_ + x] = color;
  }
}

void Context::draw_pixel(const int x, const int y, const float w_inv, const uint32_t color) {
  const bool in_screen = x >= 0 && x < width_ && y >= 0 && y < height_;

  // depth buffer is currently set up as a 1/W buffer
  if (const int i = y * width_ + x; in_screen && w_inv > depth_buffer_[i]) {
    color_buffer_[i] = color;
    depth_buffer_[i] = w_inv;
  }
}

void Context::present(SDL_Renderer& renderer, SDL_Texture& display_texture) {
  SDL_RenderClear(&renderer);

  SDL_UpdateTexture(&display_texture, nullptr, color_buffer_.data(), width_ * sizeof(uint32_t));
  SDL_RenderTexture(&renderer, &display_texture, nullptr, nullptr);

  std::ranges::fill(color_buffer_, 0xFF87CEEB);
  std::ranges::fill(depth_buffer_, 0.0f);
  SDL_RenderPresent(&renderer);
}

} // namespace render
