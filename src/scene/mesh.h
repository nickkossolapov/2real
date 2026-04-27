#pragma once
#include "math/vec2.h"
#include "math/vec3.h"
#include "render/texture.h"

#include <memory>
#include <vector>

namespace scene {

struct Face {
  int a = -1, b = -1, c = -1;
  int a_uv = -1, b_uv = -1, c_uv = -1;
};

struct Mesh {
  std::vector<math::Vec3> vertices;
  std::vector<math::Vec2> texture_uvs;
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
  std::shared_ptr<render::Texture> texture;
};

} // namespace scene
