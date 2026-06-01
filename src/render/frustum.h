#pragma once
#include "math/plane.h"

#include <array>
#include <cmath>

namespace render {

enum class FrustumPlane {
  Left,
  Right,
  Top,
  Bottom,
  Near,
  Far
};

struct Frustum {
  std::array<math::Plane, 6> planes;

  math::Plane& operator[](FrustumPlane p) { return planes[static_cast<size_t>(p)]; }
  const math::Plane& operator[](FrustumPlane p) const { return planes[static_cast<size_t>(p)]; }
};

inline Frustum make_perspective_frustum(const float fov, const float z_near, const float z_far) {
  Frustum f;
  const float a = fov / 2;
  const float c = cos(a);
  const float s = sin(a);

  f[FrustumPlane::Left] = {.point = {0, 0, 0}, .normal = {c, 0, s}};
  f[FrustumPlane::Right] = {.point = {0, 0, 0}, .normal = {-c, 0, s}};
  f[FrustumPlane::Top] = {.point = {0, 0, 0}, .normal = {0, -c, s}};
  f[FrustumPlane::Bottom] = {.point = {0, 0, 0}, .normal = {0, c, s}};
  f[FrustumPlane::Near] = {.point = {0, 0, z_near}, .normal = {0, 0, 1.0f}};
  f[FrustumPlane::Far] = {.point = {0, 0, z_far}, .normal = {0, 0, -1.0f}};

  return f;
}

} // namespace render
