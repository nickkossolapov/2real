#include "renderer/renderer.h"

#include <SDL3/SDL.h>
#include <cmath>
#include <vector>

namespace {

bool handle_events() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      return true;
    }

    if (e.type == SDL_EVENT_KEY_DOWN) {
      if (e.key.key == SDLK_ESCAPE) {
        return true;
      }
    }
  }

  return false;
}

void render_spectrum(renderer::Renderer& renderer) {
  for (int y = 0; y < renderer::window::height; y++) {
    for (int x = 0; x < renderer::window::width; x++) {
      constexpr float frequency = 0.005f;
      Uint8 r = static_cast<Uint8>(sin(frequency * x + 0.0f) * 127 + 128);
      Uint8 g = static_cast<Uint8>(sin(frequency * x + 2.094f) * 127 + 128); // 2.094 is approx 2pi/3
      Uint8 b = static_cast<Uint8>(sin(frequency * x + 4.188f) * 127 + 128); // 4.188 is approx 4pi/3

      const float brightness = 1.0f - (static_cast<float>(y) / renderer::window::height);
      r *= brightness;
      g *= brightness;
      b *= brightness;

      renderer.draw_pixel(x, y, (255 << 24) | (r << 16) | (g << 8) | b);
    }
  }
}

} // namespace

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* display_texture = nullptr;

  if (auto init_res = renderer::init(window, sdl_renderer, display_texture); init_res != renderer::InitError::None) {
    return static_cast<int>(init_res);
  }

  bool quit = false;
  auto renderer = renderer::Renderer(renderer::window::width, renderer::window::height);

  while (!quit) {
    quit = handle_events();

    render_spectrum(renderer);

    renderer.present(*sdl_renderer, *display_texture);
  }

  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
