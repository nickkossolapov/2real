#pragma once
#include <SDL3/SDL.h>

namespace engine {

namespace window {

constexpr int width = 1600;
constexpr int height = 1200;

} // namespace window

enum class InitError { None = 0, SDLInit, WindowCreate, DisplayTextureCreate, VSyncEnable };

InitError init_sdl(SDL_Window*& window,
                   SDL_Renderer*& renderer,
                   SDL_Texture*& display_texture,
                   SDL_Gamepad*& controller,
                   bool enable_v_sync);

} // namespace engine
