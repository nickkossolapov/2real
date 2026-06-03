#pragma once

#include "camera.h"
#include "input/input.h"
#include "math/common.h"

#include <algorithm>
#include <cmath>

inline void update_fps_camera(scene::Camera& camera, const input::State& input) {
  // Forward/back movement
  camera.position.x += input.move_y * -std::sin(camera.rotation.y);
  camera.position.z += input.move_y * std::cos(camera.rotation.y);
  camera.position.y += input.move_y * std::sin(camera.rotation.x);

  // Left/right movement
  camera.position.x += input.move_x * std::cos(camera.rotation.y);
  camera.position.z += input.move_x * std::sin(camera.rotation.y);

  camera.position.y += input.trigger_right - input.trigger_left;

  camera.rotation.x += input.look_y;
  camera.rotation.y += -input.look_x;

  // Clamp pitch, wrap yaw
  constexpr float max_pitch = math::deg_to_rad(89.0f);
  camera.rotation.x = std::clamp(camera.rotation.x, -max_pitch, max_pitch);
  camera.rotation.y = std::fmod(camera.rotation.y, 2.0f * std::numbers::pi_v<float>);
}