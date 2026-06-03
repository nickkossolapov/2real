#pragma once

#include "math/mat4.h"
#include "math/vec3.h"
#include "render/frustum.h"

namespace scene {

struct Camera {
  float fov;
  float aspect_ratio;
  float z_near;
  float z_far;
  math::Vec3 position;

  enum class Mode {
    Rotation,
    LookAt
  };
  Mode mode = Mode::Rotation;

  /// For Rotation mode
  math::Vec3 rotation;

  /// For LookAt mode
  math::Vec3 target;

  math::Mat4 projection() const { return math::mat4::perspective(fov, aspect_ratio, z_near, z_far); }
  render::Frustum frustum() const {
    const float fov_x = 2 * atan(tan(fov / 2) * aspect_ratio);

    return render::make_perspective_frustum(fov_x, fov, z_near, z_far);
  }

  math::Mat4 view() const {
    if (mode == Mode::LookAt) {
      constexpr math::Vec3 up = {0.0f, 1.0f, 0.0f};

      return math::mat4::look_at(position, target, up);
    }

    return math::mat4::view_from_rotation(position, rotation);
  }
};

} // namespace scene
