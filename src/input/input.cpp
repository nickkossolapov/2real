#include "input.h"

#include <SDL3/SDL.h>
#include <cmath>

namespace input {

namespace {

float get_axis_tilt(SDL_Gamepad* gamepad, const SDL_GamepadAxis axis) {
  const Sint16 value = SDL_GetGamepadAxis(gamepad, axis);

  constexpr int dead_zone = 3500;
  constexpr float end = static_cast<float>(SDL_MAX_SINT16 - dead_zone);

  if (std::abs(value) < dead_zone) {
    return 0;
  }

  const float v = value > 0 ? static_cast<float>(value - dead_zone) : static_cast<float>(value + dead_zone);

  return v / end;
}

} // namespace

bool process_input(const float dt, SDL_Gamepad* gamepad, State& input) {
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

    if (e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || e.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
      const bool pressed = e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;

      switch (e.gbutton.button) {
        case SDL_GAMEPAD_BUTTON_SOUTH: input.a_pressed = pressed; break;
        case SDL_GAMEPAD_BUTTON_EAST:  input.b_pressed = pressed; break;
        case SDL_GAMEPAD_BUTTON_WEST:  input.x_pressed = pressed; break;
        case SDL_GAMEPAD_BUTTON_NORTH: input.y_pressed = pressed; break;
        default: break;
      }
    }
  }

  if (gamepad) {
    constexpr float move_sensitivity = 1.0f;
    constexpr float look_sensitivity = 2.0f;
    constexpr float trigger_sensitivity = 1.0f;

    input.move_x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTX) * dt * move_sensitivity;
    input.move_y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTY) * dt * move_sensitivity;

    input.look_x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTX) * dt * look_sensitivity;
    input.look_y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTY) * dt * look_sensitivity;

    input.trigger_left = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) * dt * trigger_sensitivity;
    input.trigger_right = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) * dt * trigger_sensitivity;
  }

  return false;
}

} // namespace input
