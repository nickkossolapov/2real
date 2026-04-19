#pragma once

namespace input {

struct State {
  float move_x = 0.0f;
  float move_y = 0.0f;

  float look_x = 0.0f;
  float look_y = 0.0f;
};

bool process_input(float dt, State& input);

} // namespace input
