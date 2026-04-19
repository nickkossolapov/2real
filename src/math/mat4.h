#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include <array>
#include <cmath>

namespace math {

struct Mat4 {
  std::array<std::array<float, 4>, 4> m = {{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}
  }};

  Mat4 operator*(const Mat4& right) const {
    const auto& r = right.m;
    const auto& l = m;

    Mat4 out{{{0.0}}};

    // May need to manually unroll for SIMD later
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        float v = 0.0f;

        for (int k = 0; k < 4; k++) {
          v += l[i][k] * r[k][j];
        }

        out.m[i][j] = v;
      }
    }

    return out;
  }

  Vec4 operator*(const Vec4& v) const {
    Vec4 out;

    out.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    out.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    out.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    out.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

    return out;
  }

  Vec4 transform_position(const Vec3& v) const {
    return *this * Vec4(v, 1);
  }

  Vec4 transform_direction(const Vec3& v) const {
    return *this * Vec4(v, 0);
  }
};

namespace mat4 {

inline Mat4 identity() {
  return {};
}

inline Mat4 scale(const Vec3& scale_vec) {
  Mat4 m;

  m.m[0][0] = scale_vec.x;
  m.m[1][1] = scale_vec.y;
  m.m[2][2] = scale_vec.z;

  return m;
}

inline Mat4 translation(const Vec3& translation_vec) {
  Mat4 m;

  m.m[0][3] = translation_vec.x;
  m.m[1][3] = translation_vec.y;
  m.m[2][3] = translation_vec.z;

  return m;
}

inline Mat4 rotation_x(const float a) {
  Mat4 m;
  const float s = sin(a);
  const float c = cos(a);

  m.m[1][1] = c;
  m.m[1][2] = s;
  m.m[2][1] = -s;
  m.m[2][2] = c;

  return m;
}

inline Mat4 rotation_y(const float a) {
  Mat4 m;
  const float s = sin(a);
  const float c = cos(a);

  m.m[0][0] = c;
  m.m[0][2] = -s;
  m.m[2][0] = s;
  m.m[2][2] = c;

  return m;
}

inline Mat4 rotation_z(const float a) {
  Mat4 m;
  const float s = sin(a);
  const float c = cos(a);

  m.m[0][0] = c;
  m.m[0][1] = s;
  m.m[1][0] = -s;
  m.m[1][1] = c;

  return m;
}

inline Mat4 rotation(const Vec3& v) {
  return rotation_x(v.x) * rotation_y(v.y) * rotation_z(v.z);
}

inline Mat4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
  Mat4 m{{{0.0}}};

  const float a = aspect_ratio;
  const float f = 1.0 / tan(fov / 2.0);
  const float l = z_far / (z_far - z_near);

  m.m[0][0] = a * f;
  m.m[1][1] = f;
  m.m[2][2] = l;
  m.m[2][3] = -z_near * l;
  m.m[3][2] = 1.0;

  return m;
}

} // namespace mat4

} // namespace math
