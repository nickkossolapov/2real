#pragma once
#include "../math/vec3.h"

struct Face {
  int a = 0, b = 0, c = 0;
};

struct Triangle {
  Vec3 a, b, c;
};
