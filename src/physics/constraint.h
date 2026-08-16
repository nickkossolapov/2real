#pragma once
#include "body.h"

namespace physics {

struct Constraint {
  Body* a;
  Body* b;

  std::vector<std::vector<float>> GetInvM();
  void solve();
};
} // namespace physics
