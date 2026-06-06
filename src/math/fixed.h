#pragma once
#include <cmath>
#include <compare>
#include <cstdint>

namespace math {

struct Fixed {
private:
  static constexpr int32_t scale = 256;
  int32_t value_ = 0;

  struct RawTag {};

  constexpr explicit Fixed(const int32_t i, RawTag) { value_ = i; }

  constexpr static Fixed from_raw(const int r) { return Fixed{r, RawTag{}}; }

public:
  static constexpr Fixed epsilon() { return from_raw(1); }

  Fixed() = default;
  constexpr explicit Fixed(const int i) { value_ = i * scale; }
  explicit Fixed(const float f) { value_ = lrintf(f * scale); }

  constexpr Fixed operator+(const Fixed& f) const { return from_raw(value_ + f.value_); }

  constexpr Fixed operator-(const Fixed& f) const { return from_raw(value_ - f.value_); }

  constexpr Fixed operator*(const Fixed& f) const {
    const int64_t intermediate = static_cast<int64_t>(value_) * static_cast<int64_t>(f.value_);

    return from_raw(static_cast<int32_t>(intermediate / scale));
  }

  constexpr Fixed operator/(const Fixed& f) const {
    return from_raw(static_cast<int32_t>(static_cast<int64_t>(value_) * scale / f.value_));
  }

  constexpr Fixed& operator+=(const Fixed f) {
    value_ += f.value_;

    return *this;
  }

  constexpr Fixed& operator-=(const Fixed f) {
    value_ -= f.value_;

    return *this;
  }

  constexpr Fixed operator-() const { return from_raw(-value_); }

  constexpr auto operator<=>(const Fixed&) const = default; // C++20: all comparisons

  constexpr int to_int() const { return value_ / scale; }

  constexpr bool is_zero() const { return value_ == 0; }
  
  constexpr bool is_negative() const { return value_ < 0; }

  constexpr bool is_positive() const { return value_ > 0; }

  constexpr int sign() const { return (value_ > 0) - (value_ < 0); }

  constexpr Fixed abs() const { return from_raw(value_ < 0 ? -value_ : value_); }
};

inline Fixed abs(const Fixed f) {
  return f.abs();
}

} // namespace math
