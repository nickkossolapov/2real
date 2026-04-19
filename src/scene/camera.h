#pragma once

#include "../math/mat4.h"
#include "../math/vec3.h"

namespace scene {

struct Camera {
  math::Vec3 position;
  math::Vec3 rotation;

  math::Mat4 projection;
};

} // namespace render
