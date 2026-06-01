#pragma once

#include "frustum.h"
#include "triangle.h"

#include <vector>

namespace render {

void clip_triangle(const Frustum& frustum, const Triangle& triangle, std::vector<Triangle>& out);

} // namespace render
