#ifndef INC_2REAL_DISPLAY_H
#define INC_2REAL_DISPLAY_H

#include <SDL3/SDL.h>
#include <vector>

namespace renderer {

enum class InitError { None = 0, SDLInit, WindowCreate, DisplayTextureCreate };

namespace window {

constexpr int width = 800;
constexpr int height = 600;

} // namespace window

class Renderer {
public:
  Renderer(int width, int height);
  void draw_pixel(int x, int y, uint32_t color);
  void present(SDL_Renderer& renderer, SDL_Texture& display_texture);

private:
  int width_, height_;
  std::vector<uint32_t> color_buffer_;
};

InitError init(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& display_texture);

} // namespace renderer

#endif // INC_2REAL_DISPLAY_H
