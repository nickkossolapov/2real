#pragma once
#include "vec3.h"

namespace math {
struct Vec3;

struct Vec4 {
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

  explicit Vec4() {
  }

  explicit Vec4(const Vec3& v, const float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {
  }

  Vec3 to_vec3() const {
    return {x, y, z};
  }
};

} // namespace math
