#pragma once
#include <vector>

namespace render {

struct Texture {
  std::vector<uint32_t> data;
  int width;
  int height;
};

} // namespace render
