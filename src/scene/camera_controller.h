#pragma once

#include "camera.h"
#include "input/input.h"
#include "math/common.h"

#include <algorithm>
#include <cmath>

inline void update_fps_camera(scene::Camera& camera, const input::Snapshot& input) {
  constexpr float move_sensitivity = 0.01f;
  constexpr float look_sensitivity = 0.02f;
  constexpr float trigger_sensitivity = 0.01f;

  // Forward/back movement
  camera.position.x += input.move.y * -std::sin(camera.rotation.y) * move_sensitivity;
  camera.position.z += input.move.y * std::cos(camera.rotation.y) * move_sensitivity;
  camera.position.y += input.move.y * std::sin(camera.rotation.x) * move_sensitivity;

  // Left/right movement
  camera.position.x += input.move.x * std::cos(camera.rotation.y) * move_sensitivity;
  camera.position.z += input.move.x * std::sin(camera.rotation.y) * move_sensitivity;

  camera.position.y += (input.trigger_right - input.trigger_left) * trigger_sensitivity;

  camera.rotation.x += input.look.y * look_sensitivity;
  camera.rotation.y += -input.look.x * look_sensitivity;

  // Clamp pitch, wrap yaw
  constexpr float max_pitch = math::deg_to_rad(89.0f);
  camera.rotation.x = std::clamp(camera.rotation.x, -max_pitch, max_pitch);
  camera.rotation.y = std::fmod(camera.rotation.y, 2.0f * std::numbers::pi_v<float>);
}