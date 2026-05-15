#pragma once
#include "texture.h"

#include <optional>
#include <string>

namespace render {

std::optional<Texture> load_texture_file(const std::string& path);

} // namespace render