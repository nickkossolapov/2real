#pragma once

#include <cmath>
#include <numbers>

namespace math {

constexpr float epsilon = 1e-6;

constexpr float deg_to_rad(const float deg) {
  return deg * std::numbers::pi_v<float> / 180.0f;
}

template <typename T> T lerp(const T& t1, const T& t2, const float t) {
  return t1 + (t2 - t1) * t;
}

inline int floor_to_int(const float f) {
  return static_cast<int>(std::floor(f));
}

inline int ceil_to_int(const float f) {
  return static_cast<int>(std::ceil(f));
}

inline int round_to_int(const float f) {
  return static_cast<int>(std::round(f));
}

} // namespace math
