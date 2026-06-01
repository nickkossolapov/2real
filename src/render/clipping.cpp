#include "clipping.h"

#include "frustum.h"

namespace render {

void clip_against_plane(const math::Plane& plane, std::vector<ClipVertex>& out) {
  thread_local std::vector<ClipVertex> inside_vertices;
  int i = 0;

  ClipVertex current = out[0];
  ClipVertex previous = out[out.size() - 1];

  float current_dot = dot(current.pos - plane.point, plane.normal);
  float prev_dot = dot(previous.pos - plane.point, plane.normal);

  while (i <= out.size()) {
    
    i++;
  }
}

void clip_polygon(const Frustum& frustum, std::vector<ClipVertex>& out) {
  for (auto& plane : frustum.planes) {
    clip_against_plane(plane, out);
  }
}

} // namespace render