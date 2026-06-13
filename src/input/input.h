#pragma once
#include "math/vec2.h"

#include <SDL3/SDL_gamepad.h>

namespace input {

struct State {
  math::Vec2 move;
  math::Vec2 look;

  float trigger_left = 0.0f;
  float trigger_right = 0.0f;

  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;
};

bool process_input(SDL_Gamepad*, State& input);

} // namespace input
