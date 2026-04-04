#include <SDL3/SDL.h>
#include <cstdio>
#include <vector>

namespace window {
constexpr int width = 800;
constexpr int height = 600;
}

enum class InitError {
  None = 0,
  SDLInit,
  WindowCreate,
  DisplayTextureCreate
};

InitError init(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& display_texture) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init Error: %s\n", SDL_GetError());

    return InitError::SDLInit;
  }

  if (!SDL_CreateWindowAndRenderer("2real", window::width, window::height, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

    return InitError::WindowCreate;
  }

  display_texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      window::width,
      window::height);

  if (display_texture == nullptr) {
    SDL_Log("Couldn't create texture: %s", SDL_GetError());

    return InitError::DisplayTextureCreate;
  }

  return InitError::None;
}

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

void render_spectrum(std::vector<uint32_t>& buffer) {
  for (int y = 0; y < window::height; y++) {
    for (int x = 0; x < window::width; x++) {
      constexpr float frequency = 0.005f;
      Uint8 r = static_cast<Uint8>(sin(frequency * x + 0.0f) * 127 + 128);
      Uint8 g = static_cast<Uint8>(sin(frequency * x + 2.094f) * 127 + 128); // 2.094 is approx 2pi/3
      Uint8 b = static_cast<Uint8>(sin(frequency * x + 4.188f) * 127 + 128); // 4.188 is approx 4pi/3

      const float brightness = 1.0f - (static_cast<float>(y) / window::height);
      r *= brightness;
      g *= brightness;
      b *= brightness;

      buffer[y * window::width + x] = (255 << 24) | (r << 16) | (g << 8) | b;
    }
  }
}

void render(SDL_Renderer& renderer, SDL_Texture& display_texture, std::vector<uint32_t>& color_buffer) {
  SDL_RenderClear(&renderer);
  render_spectrum(color_buffer);

  SDL_UpdateTexture(&display_texture, nullptr, color_buffer.data(), window::width * sizeof(uint32_t));
  SDL_RenderTexture(&renderer, &display_texture, nullptr, nullptr);

  std::fill(color_buffer.begin(), color_buffer.end(), 0x0);
  SDL_RenderPresent(&renderer);
}

int main(int argc, char* argv[]) {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* display_texture = nullptr;

  if (auto init_res = init(window, renderer, display_texture); init_res != InitError::None) {
    return static_cast<int>(init_res);
  }

  bool quit = false;
  std::vector<uint32_t> color_buffer(window::height * window::width);

  while (!quit) {
    quit = handle_events();

    render(*renderer, *display_texture, color_buffer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
