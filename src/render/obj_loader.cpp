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
  out.z = z;

  return true;
}

bool parse_vertex_texture(std::stringstream& ss, math::Vec2& out) {
  float u, v;

  if (!(ss >> u >> v)) {
    return false;
  }
  out.x = u;
  out.y = v;

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

  out.a = vertex_indices[0].v - 1;
  out.a_uv = vertex_indices[0].vt - 1;
  out.b = vertex_indices[1].v - 1;
  out.b_uv = vertex_indices[1].vt - 1;
  out.c = vertex_indices[2].v - 1;
  out.c_uv = vertex_indices[2].vt - 1;

  return true;
};

} // namespace

bool load_obj_file(const std::string& path, scene::Mesh& out) {
  out.faces = {};
  out.vertices = {};

  if (std::ifstream obj_file(path); obj_file.is_open()) {
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
          out.vertices.push_back(v);
        }
      } else if (prefix == "vt") {
        if (math::Vec2 uv; parse_vertex_texture(ss, uv)) {
          out.texture_uvs.push_back(uv);
        }
      } else if (prefix == "f") {
        if (scene::Face f; parse_face(ss, f)) {
          out.faces.push_back(f);
        }
      }
    }

    return true;
  }

  return false;
}

} // namespace render
