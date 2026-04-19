#pragma once
#include "../math/vec4.h"

namespace render {

struct Triangle {
  math::Vec4 a, b, c;
  math::Vec3 normal;

  explicit Triangle(const math::Vec4 a, const math::Vec4 b, const math::Vec4 c)
    : a(a),
      b(b),
      c(c),
      normal(math::cross(b.xyz() - a.xyz(), c.xyz() - a.xyz()).normalized()) {
  }
};

} // namespace render
