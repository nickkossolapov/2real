#pragma once

#include "../math/mat4.h"
#include "../math/vec3.h"

namespace scene {

struct Camera {
  math::Vec3 position;
  math::Mat4 projection;

  enum class Mode { Rotation, LookAt };
  Mode mode = Mode::Rotation;

  /// For Rotation mode
  math::Vec3 rotation;

  /// For LookAt mode
  math::Vec3 target;

  math::Mat4 view() const {
    if (mode == Mode::LookAt) {
      constexpr math::Vec3 up = {0.0f, 1.0f, 0.0f};

      return math::mat4::look_at(position, target, up);
    }

    return math::mat4::view_from_rotation(position, rotation);
  }
};

} // namespace scene
