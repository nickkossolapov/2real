#pragma once
#include <array>
#include <cassert>
#include <concepts>

namespace math {

template <int N>
  requires(N > 0)
struct VecN {
  VecN() = default;

  template <typename... T>
    requires(sizeof...(T) == N && (std::convertible_to<T, float> && ...))
  explicit constexpr VecN(T... values)
      : data_{static_cast<float>(values)...} {}

  static constexpr int size() noexcept { return N; }

  float& operator[](int i) {
    assert(i >= 0 && i < N);

    return data_[i];
  }

  const float& operator[](int i) const {
    assert(i >= 0 && i < N);

    return data_[i];
  }

  VecN operator+(const VecN& v) const {
    VecN out;

    for (int i = 0; i < N; i++) {
      out[i] = data_[i] + v.data_[i];
    }

    return out;
  }

  VecN& operator+=(const VecN& v) {
    for (int i = 0; i < N; i++) {
      data_[i] += v.data_[i];
    }

    return *this;
  }

  VecN operator-(const VecN& v) const {
    VecN out;

    for (int i = 0; i < N; i++) {
      out[i] = data_[i] - v.data_[i];
    }

    return out;
  }

  VecN& operator-=(const VecN& v) {
    for (int i = 0; i < N; i++) {
      data_[i] -= v.data_[i];
    }

    return *this;
  }

  VecN operator*(const float f) const {
    VecN out;

    for (int i = 0; i < N; i++) {
      out[i] = data_[i] * f;
    }

    return out;
  }

  VecN& operator*=(const float f) {
    for (int i = 0; i < N; i++) {
      data_[i] *= f;
    }

    return *this;
  }

private:
  std::array<float, N> data_ = {};
};

template <int N>
  requires(N > 0)
constexpr float dot(const VecN<N>& v1, const VecN<N>& v2) {
  float result = 0.0f;

  for (int i = 0; i < N; i++) {
    result += v1[i] * v2[i];
  }

  return result;
}

} // namespace math
