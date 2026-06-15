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

bool read_events(Snapshot& input, KeyboardState& kb) {
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

    if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
      const ButtonState state = e.type == SDL_EVENT_KEY_DOWN ? ButtonState::Pressed : ButtonState::Released;

      switch (e.key.key) {
      case SDLK_UP:
        kb.arrow_up = state;
        break;
      case SDLK_DOWN:
        kb.arrow_down = state;
        break;
      case SDLK_RIGHT:
        kb.arrow_right = state;
        break;
      case SDLK_LEFT:
        kb.arrow_left = state;
        break;
      default:
        break;
      }
    }

    if (e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || e.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
      const ButtonState state = e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ? ButtonState::Pressed : ButtonState::Released;

      switch (e.gbutton.button) {
      case SDL_GAMEPAD_BUTTON_SOUTH:
        input.south = state;
        break;
      case SDL_GAMEPAD_BUTTON_EAST:
        input.east = state;
        break;
      case SDL_GAMEPAD_BUTTON_WEST:
        input.west = state;
        break;
      case SDL_GAMEPAD_BUTTON_NORTH:
        input.north = state;
        break;
      default:
        break;
      }
    }
  }

  return false;
}

void advance_button_state(ButtonState& state) {
  if (state == ButtonState::Pressed) {
    state = ButtonState::Down;
  } else if (state == ButtonState::Released) {
    state = ButtonState::Up;
  }
}

float resolve_axis(const ButtonState pos, const ButtonState neg, const bool gamepad_active, const float stick_value) {
  const bool is_pos_down = pos == ButtonState::Pressed || pos == ButtonState::Down;
  const bool is_neg_down = neg == ButtonState::Pressed || neg == ButtonState::Down;

  if (is_pos_down == is_neg_down) {
    return gamepad_active ? stick_value : 0;
  }

  return is_pos_down ? 1.0f : -1.0f;
}

} // namespace

bool process_input(SDL_Gamepad* gamepad, Snapshot& input, KeyboardState& kb) {
  advance_button_state(input.north);
  advance_button_state(input.south);
  advance_button_state(input.east);
  advance_button_state(input.west);

  advance_button_state(kb.arrow_up);
  advance_button_state(kb.arrow_down);
  advance_button_state(kb.arrow_left);
  advance_button_state(kb.arrow_right);

  if (read_events(input, kb)) {
    return true;
  }

  if (gamepad) {
    input.move.x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    input.move.y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

    input.look.x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    input.look.y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);

    input.trigger_left = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    input.trigger_right = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
  }

  input.move.y = resolve_axis(kb.arrow_up, kb.arrow_down, gamepad, input.move.y);
  input.move.x = resolve_axis(kb.arrow_right, kb.arrow_left, gamepad, input.move.x);

  return false;
}

} // namespace input
