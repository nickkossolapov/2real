#include "renderer.h"

#include "clipping.h"
#include "color.h"
#include "draw.h"
#include "scene/light.h"
#include "triangle.h"

#include <algorithm>
#include <vector>

namespace render {

namespace {

struct ProjectedVertex {
  math::Vec2 pos;
  float z;
  float w;
};

ProjectedVertex project(const int width, const int height, const math::Mat4& projection, const math::Vec4& pos) {
  math::Vec4 res = projection * pos;

  if (res.w != 0.0f) {
    res.x /= res.w;
    res.y /= res.w;
  }

  const float w_center = static_cast<float>(width) / 2.0f;
  const float h_center = static_cast<float>(height) / 2.0f;

  return {.pos = {res.x * w_center + w_center, -res.y * h_center + h_center}, .z = res.z, .w = res.w};
}

bool is_back_facing(const Triangle& triangle) {
  const math::Vec3 to_camera = -triangle.vertices[0].xyz();

  return math::dot(triangle.normal, to_camera) < 0;
}

Triangle make_triangle(const scene::Entity& entity, const math::Mat4& view_matrix, const scene::Face& face) {
  const math::Vec4 a = view_matrix.transform_position(entity.mesh->vertices[face.a]);
  const math::Vec4 b = view_matrix.transform_position(entity.mesh->vertices[face.b]);
  const math::Vec4 c = view_matrix.transform_position(entity.mesh->vertices[face.c]);

  if (face.a_uv != -1 && face.b_uv != -1 && face.c_uv != -1) {
    const std::array uvs = {
        entity.mesh->texture_uvs[face.a_uv],
        entity.mesh->texture_uvs[face.b_uv],
        entity.mesh->texture_uvs[face.c_uv],
    };

    return Triangle({a, b, c}, uvs);
  }

  return Triangle({a, b, c});
}

void transform_entity(const scene::Entity& entity,
                      const math::Mat4& view_matrix,
                      const Frustum& frustum,
                      std::vector<Triangle>& out) {
  out.clear();

  const math::Mat4 world_matrix = math::mat4::translation(entity.transform.position) *
                                  math::mat4::rotation(entity.transform.rotation) *
                                  math::mat4::scale(entity.transform.scale);

  const math::Mat4 mv = view_matrix * world_matrix;

  for (auto& face : entity.mesh->faces) {
    const auto triangle = make_triangle(entity, mv, face);

    if (is_back_facing(triangle)) {
      continue;
    }

    clip_triangle(frustum, triangle, out);
  }
}

uint32_t apply_light_intensity(const uint32_t color, const float intensity) {
  const uint32_t a = color & 0xFF000000;
  const uint32_t r = static_cast<uint32_t>((color >> 16 & 0xFF) * intensity);
  const uint32_t g = static_cast<uint32_t>((color >> 8 & 0xFF) * intensity);
  const uint32_t b = static_cast<uint32_t>((color & 0xFF) * intensity);

  return a | r << 16 | g << 8 | b;
}

float calculate_flat_lighting(const Triangle& triangle, const math::Vec3& light_dir) {
  constexpr float ambient = 0.1f;

  return ambient + (1.0f - ambient) * std::max(0.0f, -math::dot(triangle.normal, light_dir));
}

} // namespace

void render_entity(Framebuffer& framebuffer,
                   const scene::Scene& scn,
                   const scene::Entity& entity,
                   const RenderMode mode) {
  thread_local std::vector<Triangle> triangle_buffer;

  const math::Mat4 view = scn.camera.view();
  const Frustum frustum = scn.camera.frustum();
  const math::Vec3 view_light_dir = view.transform_direction(scn.light.direction).xyz().normalized();
  const math::Mat4 projection = scn.camera.projection();

  transform_entity(entity, view, frustum, triangle_buffer);

  const int width = framebuffer.width(), height = framebuffer.height();

  for (const Triangle& t : triangle_buffer) {
    const auto a = project(width, height, projection, t.vertices[0]);
    const auto b = project(width, height, projection, t.vertices[1]);
    const auto c = project(width, height, projection, t.vertices[2]);

    if (mode == RenderMode::Flat) {
      const float light_intensity = calculate_flat_lighting(t, view_light_dir);

      const std::array<draw::FlatVertex, 3> flat_vertices = {{
          {.pos = a.pos, .z = a.z, .w = a.w},
          {.pos = b.pos, .z = b.z, .w = b.w},
          {.pos = c.pos, .z = c.z, .w = c.w},
      }};

      const uint32_t color = apply_light_intensity(entity.flat_colour, light_intensity);

      draw::filled_triangle(framebuffer, flat_vertices, color);
    }

    if (mode == RenderMode::Textured) {
      if (entity.texture == nullptr) {
        const std::array<draw::FlatVertex, 3> flat_vertices = {{
            {.pos = a.pos, .z = a.z, .w = a.w},
            {.pos = b.pos, .z = b.z, .w = b.w},
            {.pos = c.pos, .z = c.z, .w = c.w},
        }};

        draw::filled_triangle(framebuffer, flat_vertices, color::red);
      } else {
        const std::array<draw::TexturedVertex, 3> textured_vertices = {{
            {.pos = a.pos, .uv = t.uvs[0], .z = a.z, .w = a.w},
            {.pos = b.pos, .uv = t.uvs[1], .z = b.z, .w = b.w},
            {.pos = c.pos, .uv = t.uvs[2], .z = c.z, .w = c.w},
        }};

        draw::textured_triangle(framebuffer, textured_vertices, *entity.texture);
      }
    }

    if (mode == RenderMode::Wireframe) {
      draw::line(framebuffer, a.pos, b.pos, color::white);
      draw::line(framebuffer, b.pos, c.pos, color::white);
      draw::line(framebuffer, c.pos, a.pos, color::white);
    }
  }
}

} // namespace render
