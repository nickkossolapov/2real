#include "pipeline.h"

#include "../scene/light.h"
#include "draw.h"
#include "triangle.h"

#include <algorithm>
#include <ranges>
#include <vector>

namespace render::pipeline {

namespace {

struct ProjectedVertex {
  math::Vec2 pos;
  float z;
  float w;
};

ProjectedVertex project(const Viewport& viewport, const scene::Camera& camera, const math::Vec4& v) {
  math::Vec4 res = camera.projection * v;

  if (res.w != 0.0f) {
    res.x /= res.w;
    res.y /= res.w;
  }

  const float w_center = static_cast<float>(viewport.width) / 2.0f;
  const float h_center = static_cast<float>(viewport.height) / 2.0f;

  return {.pos = {res.x * w_center + w_center, -res.y * h_center + h_center}, .z = res.z, .w = res.w};
}

bool is_front_facing(const Triangle& triangle, const math::Vec3& camera_pos) {
  const math::Vec3 to_camera = camera_pos - triangle.vertices[0].xyz();

  return math::dot(triangle.normal, to_camera) > 0;
}

void transform_entity(const scene::Entity& entity, const math::Vec3& camera_pos, std::vector<Triangle>& out) {
  out.clear();

  math::Mat4 world_matrix = math::mat4::translation(entity.transform.position) *
                            math::mat4::rotation(entity.transform.rotation) * math::mat4::scale(entity.transform.scale);

  auto make_triangle = [&entity, &world_matrix](const scene::Face& face) {
    const math::Vec4 a = world_matrix.transform_position(entity.mesh->vertices[face.a]);
    const math::Vec4 b = world_matrix.transform_position(entity.mesh->vertices[face.b]);
    const math::Vec4 c = world_matrix.transform_position(entity.mesh->vertices[face.c]);

    if (face.a_uv != -1 && face.b_uv != -1 && face.c_uv != -1) {
      const std::array uvs = {entity.mesh->texture_uvs[face.a_uv],
                              entity.mesh->texture_uvs[face.b_uv],
                              entity.mesh->texture_uvs[face.c_uv]};

      return Triangle({a, b, c}, uvs);
    }

    return Triangle({a, b, c});
  };

  auto check_back_face_culling = [&camera_pos](const Triangle& t) { return is_front_facing(t, camera_pos); };

  auto view = entity.mesh->faces | std::views::transform(make_triangle) | std::views::filter(check_back_face_culling);

  std::ranges::copy(view, std::back_inserter(out));
}

uint32_t apply_light_intensity(const uint32_t color, const float intensity) {
  const uint32_t a = color & 0xFF000000;
  const uint32_t r = static_cast<uint32_t>((color >> 16 & 0xFF) * intensity);
  const uint32_t g = static_cast<uint32_t>((color >> 8 & 0xFF) * intensity);
  const uint32_t b = static_cast<uint32_t>((color & 0xFF) * intensity);

  return a | (r << 16) | (g << 8) | b;
}

float calculate_flat_lighting(const Triangle& triangle, const scene::DirectionalLight& light) {
  constexpr float ambient = 0.1f;

  return ambient + (1.0f - ambient) * std::max(0.0f, -math::dot(triangle.normal, light.direction));
};

} // namespace

void render_entity(Context& context,
                   const Viewport& viewport,
                   const scene::Entity& entity,
                   const scene::Camera& camera,
                   const scene::DirectionalLight& light) {
  thread_local std::vector<Triangle> triangle_buffer;
  transform_entity(entity, camera.position, triangle_buffer);

  for (const Triangle& t : triangle_buffer) {
    const auto a = project(viewport, camera, t.vertices[0]);
    const auto b = project(viewport, camera, t.vertices[1]);
    const auto c = project(viewport, camera, t.vertices[2]);

    const float light_intensity = calculate_flat_lighting(t, light);
    const uint32_t color = apply_light_intensity(0xFFFFFFFF, light_intensity);

    if (entity.texture != nullptr) {
      const std::array<draw::TexturedVertex, 3> textured_vertices = {
          {{.pos = a.pos, .uv = t.uvs[0], .z = a.z, .w = a.w},
           {.pos = b.pos, .uv = t.uvs[1], .z = b.z, .w = b.w},
           {.pos = c.pos, .uv = t.uvs[2], .z = c.z, .w = c.w}}};

      draw::textured_triangle(context, textured_vertices, *entity.texture);
    } else {
      const std::array<draw::FlatVertex, 3> flat_vertices = {
          {{.pos = a.pos, .z = a.z, .w = a.w}, {.pos = b.pos, .z = b.z, .w = b.w}, {.pos = c.pos, .z = c.z, .w = c.w}}};

      draw::filled_triangle(context, flat_vertices, color);
    }
  }
}

} // namespace render::pipeline
