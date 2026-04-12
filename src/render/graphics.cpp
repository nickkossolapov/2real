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

  SDL_UpdateTexture(&display_texture, nullptr, color_buffer_.data(), window::width * sizeof(uint32_t));
  SDL_RenderTexture(&renderer, &display_texture, nullptr, nullptr);

  std::fill(color_buffer_.begin(), color_buffer_.end(), 0x0);
  SDL_RenderPresent(&renderer);
}

InitError init_sdl(SDL_Window*& window,
                   SDL_Renderer*& renderer,
                   SDL_Texture*& display_texture,
                   const bool enable_v_sync) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init Error: %s\n", SDL_GetError());

    return InitError::SDLInit;
  }

  if (!SDL_CreateWindowAndRenderer("2real", window::width, window::height, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

    return InitError::WindowCreate;
  }

  if (enable_v_sync) {
    if (SDL_SetRenderVSync(renderer, 1) == false) {
      SDL_Log("Could not enable VSync! SDL error: %s\n", SDL_GetError());

      return InitError::VSyncEnable;
    }
  }

  display_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window::width, window::height);

  if (display_texture == nullptr) {
    SDL_Log("Couldn't create texture: %s", SDL_GetError());

    return InitError::DisplayTextureCreate;
  }

  return InitError::None;
}

} // namespace graphics
