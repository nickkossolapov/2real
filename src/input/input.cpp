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

bool read_events(InputState& state, InputEvents& events, KeyboardState& kb) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      return true;
    }

    if (e.type == SDL_EVENT_MOUSE_MOTION) {
      state.cursor_position = {e.motion.x, e.motion.y};
    }

    if (e.type == SDL_EVENT_KEY_DOWN) {
      if (e.key.key == SDLK_ESCAPE) {
        return true;
      }
    }

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
      const ButtonState button = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? ButtonState::Down : ButtonState::Up;
      const Event event = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? Event::Pressed : Event::Released;

      switch (e.button.button) {
      case SDL_BUTTON_LEFT:
        state.primary = button;
        events.primary = event;
        break;
      case SDL_BUTTON_MIDDLE:
        state.tertiary = button;
        events.tertiary = event;
        break;
      case SDL_BUTTON_RIGHT:
        state.secondary = button;
        events.secondary = event;
        break;
      default:
        break;
      }
    }

    if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
      const ButtonState button = e.type == SDL_EVENT_KEY_DOWN ? ButtonState::Down : ButtonState::Up;

      switch (e.key.key) {
      case SDLK_UP:
        kb.arrow_up = button;
        break;
      case SDLK_DOWN:
        kb.arrow_down = button;
        break;
      case SDLK_RIGHT:
        kb.arrow_right = button;
        break;
      case SDLK_LEFT:
        kb.arrow_left = button;
        break;
      default:
        break;
      }
    }

    if (e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || e.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
      const ButtonState button = e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ? ButtonState::Down : ButtonState::Up;
      const Event event = e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ? Event::Pressed : Event::Released;

      switch (e.gbutton.button) {
      case SDL_GAMEPAD_BUTTON_SOUTH:
        state.primary = button;
        events.primary = event;
        break;
      case SDL_GAMEPAD_BUTTON_EAST:
        state.secondary = button;
        events.secondary = event;
        break;
      case SDL_GAMEPAD_BUTTON_WEST:
        state.tertiary = button;
        events.tertiary = event;
        break;
      case SDL_GAMEPAD_BUTTON_NORTH:
        state.quaternary = button;
        events.quaternary = event;
        break;
      default:
        break;
      }
    }
  }

  return false;
}

float resolve_axis(const ButtonState pos, const ButtonState neg, const bool gamepad_active, const float stick_value) {
  const bool is_pos_down = pos == ButtonState::Down;
  const bool is_neg_down = neg == ButtonState::Down;

  if (is_pos_down == is_neg_down) {
    return gamepad_active ? stick_value : 0;
  }

  return is_pos_down ? 1.0f : -1.0f;
}

void clear_transitions(InputEvents& events) {
  events.primary = Event::None;
  events.secondary = Event::None;
  events.tertiary = Event::None;
  events.quaternary = Event::None;
}

} // namespace

bool process_input(SDL_Gamepad* gamepad, InputState& state, InputEvents& events, KeyboardState& kb) {
  clear_transitions(events);

  if (read_events(state, events, kb)) {
    return true;
  }

  if (gamepad) {
    state.move.x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    state.move.y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

    state.look.x = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    state.look.y = -get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);

    state.trigger_left = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    state.trigger_right = get_axis_tilt(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
  }

  state.move.y = resolve_axis(kb.arrow_up, kb.arrow_down, gamepad, state.move.y);
  state.move.x = resolve_axis(kb.arrow_right, kb.arrow_left, gamepad, state.move.x);

  return false;
}

} // namespace input
