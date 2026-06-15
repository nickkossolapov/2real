#pragma once
#include "math/vec2.h"

#include <SDL3/SDL_gamepad.h>

namespace input {

enum class ButtonState {
  Up,
  Pressed,
  Down,
  Released
};

struct Snapshot {
  math::Vec2 move;
  math::Vec2 look;
  math::Vec2 cursor_position;

  float trigger_left = 0.0f, trigger_right = 0.0f;

  ButtonState north = ButtonState::Up;
  ButtonState south = ButtonState::Up;
  ButtonState east = ButtonState::Up;
  ButtonState west = ButtonState::Up;
};

struct KeyboardState {
  ButtonState arrow_up = ButtonState::Up;
  ButtonState arrow_down = ButtonState::Up;
  ButtonState arrow_left = ButtonState::Up;
  ButtonState arrow_right = ButtonState::Up;
};

bool process_input(SDL_Gamepad*, Snapshot& input, KeyboardState& kb);

} // namespace input
