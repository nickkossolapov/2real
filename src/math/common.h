#pragma once
#include <numbers>

namespace math {

constexpr float deg_to_rad(const float deg) {
  return deg * std::numbers::pi_v<float> / 180.0f;
}

} // namespace math
