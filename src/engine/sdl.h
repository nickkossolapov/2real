#pragma once
#include <memory>
#include <SDL3/SDL.h>

namespace engine {

namespace window {

constexpr int width = 1600;
constexpr int height = 1200;

} // namespace window

enum class InitError { None = 0, SDLInit, WindowCreate, DisplayTextureCreate, VSyncEnable };

namespace sdl {

struct WindowDeleter {
  void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
};

struct RendererDeleter {
  void operator()(SDL_Renderer* p) const { SDL_DestroyRenderer(p); }
};

struct TextureDeleter {
  void operator()(SDL_Texture* p) const { SDL_DestroyTexture(p); }
};

struct GamepadDeleter {
  void operator()(SDL_Gamepad* p) const { SDL_CloseGamepad(p); }
};

using Window = std::unique_ptr<SDL_Window, WindowDeleter>;
using Renderer = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using Texture = std::unique_ptr<SDL_Texture, TextureDeleter>;
using Gamepad = std::unique_ptr<SDL_Gamepad, GamepadDeleter>;

} // namespace sdl

class SdlContext {
public:
  SdlContext() = default;
  ~SdlContext();

  SdlContext(const SdlContext&) = delete;
  SdlContext& operator=(const SdlContext&) = delete;
  SdlContext(SdlContext&&) = delete;
  SdlContext& operator=(SdlContext&&) = delete;

  InitError init(bool enable_v_sync);

  SDL_Renderer& renderer() const { return *renderer_; }
  SDL_Texture& display_texture() const { return *display_texture_; }

private:
  sdl::Window window_;
  sdl::Renderer renderer_;
  sdl::Texture display_texture_;
  sdl::Gamepad controller_;
};

} // namespace engine
