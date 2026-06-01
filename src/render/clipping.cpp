#include "clipping.h"

#include "frustum.h"
#include "math/common.h"
#include "math/vec3.h"

namespace render {

namespace {

struct ClipVertex {
  math::Vec3 pos;
};

void clip_against_plane(const math::Plane& plane, const std::vector<ClipVertex>& in, std::vector<ClipVertex>& out) {
  if (in.empty()) {
    return;
  }

  ClipVertex prev = in.back();
  float prev_dot = dot(prev.pos - plane.point, plane.normal);

  for (const auto& current : in) {
    const float current_dot = dot(current.pos - plane.point, plane.normal);

    if (current_dot * prev_dot < 0) {
      const float t = current_dot / (current_dot - prev_dot);

      out.push_back({.pos = math::lerp(current.pos, prev.pos, t)});
    }

    if (current_dot >= 0) {
      out.push_back(current);
    }

    prev = current;
    prev_dot = current_dot;
  }
}

} // namespace

void clip_triangle(const Frustum& frustum, const Triangle& triangle, std::vector<Triangle>& out) {
  thread_local std::vector<ClipVertex> a, b;
  a.clear();

  for (const auto& vertex : triangle.vertices) {
    a.push_back({.pos = vertex.xyz()}); // TODO UVs
  }

  auto* in_buffer = &a;
  auto* out_buffer = &b;

  for (const auto& plane : frustum.planes) {
    out_buffer->clear();
    clip_against_plane(plane, *in_buffer, *out_buffer);
    std::swap(in_buffer, out_buffer);

    if (in_buffer->empty()) {
      return;
    }
  }

  // fan_triangulate(*in_buf, out);
}

} // namespace render