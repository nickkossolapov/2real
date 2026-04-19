#pragma once
#include "vec2.h"
#include "vec3.h"

namespace math {

struct Vec4 {
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

  explicit Vec4() {
  }

  explicit Vec4(const Vec3& v, const float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {
  }

  Vec3 xyz() const {
    return {x, y, z};
  }

  Vec2 xy() const {
    return {x, y};
  }
};

} // namespace math
