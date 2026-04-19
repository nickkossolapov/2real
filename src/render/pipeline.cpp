#include "pipeline.h"

#include "draw.h"
#include "triangle.h"

#include <algorithm>
#include <ranges>
#include <vector>

namespace render::pipeline {

namespace {

math::Vec2 project(const Camera& camera, const math::Vec4& v) {
  math::Vec4 res = camera.projection * v;

  if (res.w != 0.0f) {
    res.x /= res.w;
    res.y /= res.w;
  }

  return {
      res.x * 400.0f + static_cast<float>(graphics::window::width) / 2.0f,
      res.y * 300.0f + static_cast<float>(graphics::window::height) / 2.0f
  };
}

bool is_front_facing(const Triangle& triangle, const math::Vec3& camera_pos) {
  const math::Vec3 normal = math::cross(triangle.b.xyz() - triangle.a.xyz(), triangle.c.xyz() - triangle.a.xyz());
  const math::Vec3 to_camera = camera_pos - triangle.a.xyz();

  return math::dot(normal, to_camera) > 0;
}

void sort_by_depth(std::vector<Triangle>& triangles) {
  auto comparer = [](const Triangle& l, const Triangle& r) {
    const float z_l = l.a.z + l.b.z + l.c.z;
    const float z_r = r.a.z + r.b.z + r.c.z;

    return z_l > z_r;
  };

  std::ranges::sort(triangles.begin(), triangles.end(), comparer);
}

std::vector<Triangle> transform_entity(const Entity& entity, const math::Vec3& camera_pos) {
  math::Mat4 world_matrix =
      math::mat4::translation(entity.transform.position)
      * math::mat4::rotation(entity.transform.rotation)
      * math::mat4::scale(entity.transform.scale);

  auto make_triangle = [&entity, &world_matrix](const Face& face) {
    const math::Vec4 a = world_matrix.transform_position(entity.mesh->vertices[face.a]);
    const math::Vec4 b = world_matrix.transform_position(entity.mesh->vertices[face.b]);
    const math::Vec4 c = world_matrix.transform_position(entity.mesh->vertices[face.c]);

    return Triangle(a, b, c);
  };
  auto check_back_face_culling = [&camera_pos](const Triangle& t) { return is_front_facing(t, camera_pos); };

  auto view =
      entity.mesh->faces
      | std::views::transform(make_triangle)
      | std::views::filter(check_back_face_culling);

  auto materialized = std::vector(view.begin(), view.end());
  sort_by_depth(materialized);

  return materialized;
}


}

void render_entity(graphics::Context& context,
                   const Entity& entity,
                   const Camera& camera) {

  for (const auto triangles = transform_entity(entity, camera.position); const Triangle& t : triangles) {
    const math::Vec2 a = project(camera, t.a);
    const math::Vec2 b = project(camera, t.b);
    const math::Vec2 c = project(camera, t.c);

    draw::filled_triangle(context, a, b, c, 0xFFFFFFFF);
    draw::triangle(context, a, b, c, 0xFF000000);
  }
}

}
