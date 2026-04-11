#pragma once
#include "../math/vec3.h"

#include <vector>

struct Face {
  int a = 0, b = 0, c = 0;
};

struct Triangle {
  Vec3 a, b, c;
};

struct Mesh {
  std::vector<Vec3> vertices;
  std::vector<Face> faces;
  Vec3 rotation;
};
