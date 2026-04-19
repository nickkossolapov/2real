#pragma once
#include "../math/vec3.h"

namespace scene {

struct DirectionalLight {
  math::Vec3 direction;

  explicit DirectionalLight(const math::Vec3& dir) : direction(dir.normalized()) {
  }
};

} // namespace scene
