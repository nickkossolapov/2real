#pragma once
#include "vec_n.h"

#include <array>
#include <cassert>
#include <concepts>

namespace math {

template <int Rows, int Cols>
  requires(Rows > 0 && Cols > 0)
struct MatMN {
  MatMN() = default;

  template <typename... T>
    requires(sizeof...(T) == Rows * Cols && (std::convertible_to<T, float> && ...))
  explicit constexpr MatMN(T... values)
      : data_{static_cast<float>(values)...} {}

  static constexpr int size() noexcept { return Rows * Cols; }

  float& operator()(const int row, const int col) {
    const int i = row * Cols + col;

    assert(i >= 0 && i < size());
    assert(row >= 0 && row < Rows);
    assert(col >= 0 && col < Cols);

    return data_[i];
  }

  const float& operator()(const int row, const int col) const {
    const int i = row * Cols + col;

    assert(i >= 0 && i < size());
    assert(row >= 0 && row < Rows);
    assert(col >= 0 && col < Cols);

    return data_[i];
  }

  void zero() { data_.fill(0.0f); }

  MatMN<Cols, Rows> transpose() const {
    MatMN<Cols, Rows> out;

    for (int i = 0; i < Rows; ++i) {
      for (int j = 0; j < Cols; ++j) {
        out(j, i) = (*this)(i, j);
      }
    }

    return out;
  }

  VecN<Rows> operator*(const VecN<Cols>& v) const {
    VecN<Rows> out;

    // todo

    return out;
  }

  template <int OtherCols>
    requires(OtherCols > 0)
  MatMN<Rows, OtherCols> operator*(const MatMN<Cols, OtherCols>& r) const {
    const MatMN& l = *this;
    MatMN<Rows, OtherCols> out;

    // todo

    return out;
  }

private:
  std::array<float, Rows * Cols> data_ = {};
};

} // namespace math
