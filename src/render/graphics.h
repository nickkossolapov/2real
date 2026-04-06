#pragma once

#include <vector>
#include <SDL3/SDL.h>

namespace graphics {

enum class InitError { None = 0, SDLInit, WindowCreate, DisplayTextureCreate, VSyncEnable };

namespace window {

constexpr int width = 800;
constexpr int height = 600;

} // namespace window

class Context {
public:
  Context(int width, int height);
  void draw_pixel(int x, int y, uint32_t color);
  void present(SDL_Renderer& renderer, SDL_Texture& display_texture);

private:
  int width_, height_;
  std::vector<uint32_t> color_buffer_;
};

InitError init_sdl(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& display_texture, bool enable_v_sync);

} // namespace graphics
