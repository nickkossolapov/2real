#pragma once
#include <SDL3/SDL_gamepad.h>

namespace input {

struct State {
  float move_x = 0.0f;
  float move_y = 0.0f;

  float look_x = 0.0f;
  float look_y = 0.0f;

  float trigger_left = 0.0f;
  float trigger_right = 0.0f;
};

bool process_input(float dt, SDL_Gamepad*, State& input);

} // namespace input
