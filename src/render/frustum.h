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

inline Frustum make_perspective_frustum(const float fov_x, const float fov_y, const float z_near, const float z_far) {
  Frustum f;
  const float a_x = fov_x / 2;
  const float a_y = fov_y / 2;
  const float c_x = cos(a_x);
  const float c_y = cos(a_y);
  const float s_x = sin(a_x);
  const float s_y = sin(a_y);

  f[FrustumPlane::Left] = {.point = {0, 0, 0}, .normal = {c_x, 0, s_x}};
  f[FrustumPlane::Right] = {.point = {0, 0, 0}, .normal = {-c_x, 0, s_x}};
  f[FrustumPlane::Top] = {.point = {0, 0, 0}, .normal = {0, -c_y, s_y}};
  f[FrustumPlane::Bottom] = {.point = {0, 0, 0}, .normal = {0, c_y, s_y}};
  f[FrustumPlane::Near] = {.point = {0, 0, z_near}, .normal = {0, 0, 1.0f}};
  f[FrustumPlane::Far] = {.point = {0, 0, z_far}, .normal = {0, 0, -1.0f}};

  return f;
}

} // namespace render
