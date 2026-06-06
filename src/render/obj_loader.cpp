#include "obj_loader.h"

#include <fstream>
#include <sstream>

namespace render {

namespace {

struct FaceIndices {
  int v = -1, vt = -1, vn = -1;
};

bool parse_vertex(std::stringstream& ss, math::Vec3& out) {
  float x, y, z;

  if (!(ss >> x >> y >> z)) {
    return false;
  }

  out.x = x;
  out.y = y;

  // This engine is left-handed (+z into the screen), but .obj assets are generally right-handed(+z out of the screen).
  // Negate z on import to convert into left-handed space.
  out.z = -z;

  return true;
}

// Used to tile textures, so when vt is not in range [0, 1]
float wrap(const float v) {
  const float r = v - std::floor(v);

  return r == 0.0f && v > 0.0f ? 1.0f : r;
}

// .obj indices are 1-based; 0 means "absent" in a face token. Convert to 0-based, or -1 if absent.
int to_index(const int i) {
  return i > 0 ? i - 1 : -1;
}

bool parse_vertex_texture(std::stringstream& ss, math::Vec2& out) {
  float u, v;

  if (!(ss >> u >> v)) {
    return false;
  }
  out.x = wrap(u);
  out.y = wrap(1.0f - v); // obj files have UV origin in bottom-left, while textures are top-left

  return true;
}

bool parse_face_token(const std::string& token, FaceIndices& indices) {
  std::stringstream ss(token);
  std::string segment;
  int part = 0;

  while (getline(ss, segment, '/')) {
    if (!segment.empty()) {
      try {
        const int val = std::stoi(segment);

        if (part == 0) {
          indices.v = val;
        } else if (part == 1) {
          indices.vt = val;
        } else if (part == 2) {
          indices.vn = val;
        }
      } catch (const std::invalid_argument&) {
        return false;
      } catch (const std::out_of_range&) {
        return false;
      }
    }

    part++;
  }

  return true;
}

bool parse_face(std::stringstream& ss, scene::Face& out) {
  std::string token;
  std::vector<FaceIndices> vertex_indices{};

  while (ss >> token) {
    if (FaceIndices i; parse_face_token(token, i)) {
      vertex_indices.push_back(i);
    } else {
      return false;
    }
  }

  if (vertex_indices.size() < 3) {
    return false;
  }

  // Vertices are mirrored on import (z negated to convert right-handed .obj data into left-handed space).
  // So reverse the winding order (swap b and c) to keep front faces counter-clockwise for backface culling.
  out.a = vertex_indices[0].v - 1;
  out.a_uv = to_index(vertex_indices[0].vt);
  out.b = vertex_indices[2].v - 1;
  out.b_uv = to_index(vertex_indices[2].vt);
  out.c = vertex_indices[1].v - 1;
  out.c_uv = to_index(vertex_indices[1].vt);

  return true;
};

} // namespace

std::optional<scene::Mesh> load_obj_file(const std::string& path) {

  if (std::ifstream obj_file(path); obj_file.is_open()) {
    scene::Mesh m;
    std::string line;

    while (getline(obj_file, line)) {
      if (line.length() <= 1) {
        continue;
      }

      std::string prefix;
      std::stringstream ss(line);

      ss >> prefix;

      if (prefix == "v") {
        if (math::Vec3 v; parse_vertex(ss, v)) {
          m.vertices.push_back(v);
        }
      } else if (prefix == "vt") {
        if (math::Vec2 uv; parse_vertex_texture(ss, uv)) {
          m.texture_uvs.push_back(uv);
        }
      } else if (prefix == "f") {
        if (scene::Face f; parse_face(ss, f)) {
          m.faces.push_back(f);
        }
      }
    }

    return m;
  }

  return {};
}

} // namespace render
