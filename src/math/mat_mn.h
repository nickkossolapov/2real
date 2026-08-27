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
    assert(row >= 0 && row < Rows);
    assert(col >= 0 && col < Cols);

    const int i = row * Cols + col;
    return data_[i];
  }

  const float& operator()(const int row, const int col) const {
    assert(row >= 0 && row < Rows);
    assert(col >= 0 && col < Cols);

    const int i = row * Cols + col;
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

    for (int row = 0; row < Rows; ++row) {
      for (int col = 0; col < Cols; ++col) {
        out[row] += (*this)(row, col) * v[col];
      }
    }

    return out;
  }

  template <int OtherCols>
    requires(OtherCols > 0)
  MatMN<Rows, OtherCols> operator*(const MatMN<Cols, OtherCols>& right) const {
    MatMN<Rows, OtherCols> out;

    for (int row = 0; row < Rows; ++row) {
      for (int col = 0; col < OtherCols; ++col) {
        for (int i = 0; i < Cols; ++i) {
          out(row, col) += (*this)(row, i) * right(i, col);
        }
      }
    }

    return out;
  }

private:
  std::array<float, Rows * Cols> data_ = {};
};

template <int N>
  requires(N > 0)
VecN<N> solve_linear_system(const MatMN<N, N>& a,
                            const VecN<N>& b,
                            const VecN<N> initial = {},
                            const int iterations = 8) {
  VecN<N> x = initial;

  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < N; ++i) {
      float term = 0;

      for (int j = 0; j < N; ++j) {
        if (j != i) {
          term += a(i, j) * x[j];
        }
      }

      x[i] = (b[i] - term) / a(i, i);
    }
  }

  return x;
}

} // namespace math
