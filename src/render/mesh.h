#pragma once
#include "triangle.h"
#include "../math/vec3.h"

#include <vector>

struct Mesh {
  std::vector<Vec3> vertices;
  std::vector<Face> faces;
  Vec3 rotation;
};
