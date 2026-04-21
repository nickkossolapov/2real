#pragma once
#include "../math/vec4.h"

namespace render {

struct Triangle {
  std::array<math::Vec4, 3> vertices;
  std::array<math::Vec2, 3> uvs;
  math::Vec3 normal;

  Triangle(const std::array<math::Vec4, 3>& v, const std::array<math::Vec2, 3>& uvs)
      : vertices(v),
        uvs(uvs),
        normal(math::cross(v[1].xyz() - v[0].xyz(), v[2].xyz() - v[0].xyz()).normalized()) {}

  explicit Triangle(const std::array<math::Vec4, 3>& v)
      : Triangle(v, {}) {}
};

} // namespace render
