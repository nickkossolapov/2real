#pragma once
#include "math/vec3.h"

#include <vector>

namespace render {

struct ClipVertex {
  math::Vec3 pos;
};

void clip_polygon(std::vector<ClipVertex>& out);

} // namespace render
