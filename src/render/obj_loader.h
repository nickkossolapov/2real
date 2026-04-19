#pragma once
#include "../scene/mesh.h"

#include <string>

namespace render {

bool load_obj_file(const std::string& path, scene::Mesh& out);

} // namespace render
