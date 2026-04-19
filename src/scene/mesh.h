#pragma once
#include "../math/vec3.h"

#include <memory>
#include <vector>

namespace scene {

struct Face {
  int a = 0, b = 0, c = 0;
};

struct Mesh {
  std::vector<math::Vec3> vertices;
  std::vector<Face> faces;
};

struct Transform {
  math::Vec3 position;
  math::Vec3 rotation;
  math::Vec3 scale = {1.0f, 1.0f, 1.0f};
};

struct Entity {
  std::shared_ptr<Mesh> mesh;
  Transform transform;
};

} // namespace render
