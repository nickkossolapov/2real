#include "graphics.h"

namespace graphics {

Context::Context(const int width, const int height)
  : width_(width),
    height_(height),
    color_buffer_(height * width) {
}

void Context::draw_pixel(const int x, const int y, const uint32_t color) {
  if (x >= 0 && x < width_ && y >= 0 && y < height_) {
    color_buffer_[y * width_ + x] = color;
  }
}

void Context::present(SDL_Renderer& renderer, SDL_Texture& display_texture) {
  SDL_RenderClear(&renderer);

  SDL_UpdateTexture(&display_texture, nullptr, color_buffer_.data(), width_ * sizeof(uint32_t));
  SDL_RenderTexture(&renderer, &display_texture, nullptr, nullptr);

  std::fill(color_buffer_.begin(), color_buffer_.end(), 0x0);
  SDL_RenderPresent(&renderer);
}

} // namespace graphics
