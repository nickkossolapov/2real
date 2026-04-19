#include "input.h"

#include <cmath>
#include <SDL3/SDL.h>

namespace input {

namespace {

float get_axis_tilt(const Sint16 value) {
  constexpr int dead_zone = 3500;
  constexpr float end = static_cast<float>(SDL_MAX_SINT16 - dead_zone);

  if (std::abs(value) < dead_zone) {
    return 0;
  }

  const float v = value > 0 ? static_cast<float>(value - dead_zone) : static_cast<float>(value + dead_zone);

  return v / end;
}

}

bool process_input(float dt, State& input) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      return true;
    }

    if (e.type == SDL_EVENT_KEY_DOWN) {
      if (e.key.key == SDLK_ESCAPE) {
        return true;
      }
    } else if (e.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
      if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
        input.move_x = get_axis_tilt(e.gaxis.value);
      }
      if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
        input.move_y = get_axis_tilt(e.gaxis.value);
      }
      if (e.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTX) {
        input.look_x = get_axis_tilt(e.gaxis.value);
      }
      if (e.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTY) {
        input.look_y = get_axis_tilt(e.gaxis.value);
      }
    }
  }

  return false;
}

} // namespace input
