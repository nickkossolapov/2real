#pragma once

#include <cmath>
#include <numbers>

namespace math {

constexpr float deg_to_rad(const float deg) {
  return deg * std::numbers::pi_v<float> / 180.0f;
}

template <typename T> T lerp(const T& t1, const T& t2, const float t) {
  return t1 + (t2 - t1) * t;
}

} // namespace math
