#include "obj_loader.h"

#include <fstream>
#include <sstream>

using namespace std;

namespace {

struct FaceIndices {
  int v = 0, vt = 0, vn = 0;
};

bool parse_vertex(stringstream& ss, Vec3& out) {
  float x, y, z;

  if (!(ss >> x >> y >> z)) {
    return false;
  }

  out.x = x;
  out.y = y;
  out.z = z;

  return true;
};

bool parse_face_token(const string& token, FaceIndices& indices) {
  stringstream ss(token);
  string segment;
  int part = 0;

  while (getline(ss, segment, '/')) {
    if (!segment.empty()) {
      try {
        const int val = stoi(segment);

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

bool parse_face(stringstream& ss, Face& out) {
  string token;
  vector<FaceIndices> vertex_indices{};

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
  out.b = vertex_indices[1].v - 1;
  out.c = vertex_indices[2].v - 1;

  return true;
};

}

bool load_obj_file(const string& path, Mesh& out) {
  out.faces = {};
  out.vertices = {};

  if (ifstream obj_file(path); obj_file.is_open()) {
    string line;

    while (getline(obj_file, line)) {
      if (line.length() <= 1) {
        continue;
      }

      string prefix;
      stringstream ss(line);

      ss >> prefix;

      if (prefix == "v") {
        if (Vec3 v; parse_vertex(ss, v)) {
          out.vertices.push_back(v);
        }
      } else if (prefix == "f") {
        if (Face f; parse_face(ss, f)) {
          out.faces.push_back(f);
        }
      }
    }

    return true;
  }

  return false;
}
