#pragma once
#include "scene/mesh.h"

#include <optional>
#include <string>

namespace render {

std::optional<scene::Mesh> load_obj_file(const std::string& path);

} // namespace render
