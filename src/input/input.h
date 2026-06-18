#pragma once
#include "math/vec2.h"

#include <SDL3/SDL_gamepad.h>

namespace input {

enum class ButtonState {
  Up,
  Down,
};

enum class Event {
  None,
  Pressed,
  Released
};

struct InputEvents {
  Event primary = Event::None;
  Event secondary = Event::None;
  Event tertiary = Event::None;
  Event quaternary = Event::None;
};

struct InputState {
  math::Vec2 move;
  math::Vec2 look;
  math::Vec2 cursor_position;

  float trigger_left = 0.0f, trigger_right = 0.0f;

  ButtonState primary = ButtonState::Up;
  ButtonState secondary = ButtonState::Up;
  ButtonState tertiary = ButtonState::Up;
  ButtonState quaternary = ButtonState::Up;
};

struct KeyboardState {
  ButtonState arrow_up = ButtonState::Up;
  ButtonState arrow_down = ButtonState::Up;
  ButtonState arrow_left = ButtonState::Up;
  ButtonState arrow_right = ButtonState::Up;
};

bool process_input(SDL_Gamepad*, InputState& state, InputEvents& events, KeyboardState& kb);

} // namespace input
