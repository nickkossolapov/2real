#include "clipping.h"

#include "frustum.h"
#include "math/common.h"
#include "math/vec3.h"

namespace render {

namespace {

struct ClipVertex {
  math::Vec3 pos;
  math::Vec2 uv;
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

      const ClipVertex intersection = {
          .pos = math::lerp(current.pos, prev.pos, t),
          .uv = math::lerp(current.uv, prev.uv, t),
      };

      out.push_back(intersection);
    }

    if (current_dot >= 0) {
      out.push_back(current);
    }

    prev = current;
    prev_dot = current_dot;
  }
}

void fan_triangulate(const std::vector<ClipVertex>& clip_buffer, std::vector<Triangle>& out) {
  if (clip_buffer.size() < 3) {
    return;
  }

  for (int i = 1; i < clip_buffer.size() - 1; i++) {
    Triangle new_triangle(
        {math::Vec4(clip_buffer[0].pos, 1), math::Vec4(clip_buffer[i].pos, 1), math::Vec4(clip_buffer[i + 1].pos, 1)},
        {clip_buffer[0].uv, clip_buffer[i].uv, clip_buffer[i + 1].uv});

    out.push_back(new_triangle);
  }
}

} // namespace

void clip_triangle(const Frustum& frustum, const Triangle& triangle, std::vector<Triangle>& out) {
  thread_local std::vector<ClipVertex> a, b;
  a.clear();

  for (int i = 0; i < 3; i++) {
    a.push_back({
        .pos = triangle.vertices[i].xyz(),
        .uv = triangle.uvs[i],
    });
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

  fan_triangulate(*in_buffer, out);
}

} // namespace render