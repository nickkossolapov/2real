#pragma once

#include "../math/mat4.h"
#include "../math/vec3.h"

namespace scene {

struct Camera {
  math::Vec3 position;
  // math::Vec3 rotation;
  math::Vec3 direction;

  math::Mat4 projection;
  math::Mat4 view() const {
    constexpr math::Vec3 up = {0.0f, 1.0f, 0.0f};

    return math::mat4::look_at(position, {0.0f, 0.0f, 5.0f}, up);
  };
};

} // namespace scene
