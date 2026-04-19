#pragma once
#include "../scene/mesh.h"

#include <string>

bool load_obj_file(const std::string& path, scene::Mesh& out);
