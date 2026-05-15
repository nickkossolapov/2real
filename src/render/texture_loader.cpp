#include "texture_loader.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

namespace render {

std::optional<Texture> load_texture_file(const std::string& path) {
  constexpr int desired_channels = 4; // RGBA
  int w, h, channels_in_file;

  auto* data = stbi_load(path.c_str(), &w, &h, &channels_in_file, desired_channels);

  if (data == nullptr) {
    return {};
  }

  const int total_pixels = w * h;
  Texture t = {.data = std::vector<uint32_t>(total_pixels), .width = w, .height = h};

  for (int i = 0; i < total_pixels; i++) {
    const int c = i * desired_channels;
    const uint32_t r = data[c], g = data[c + 1], b = data[c + 2], a = data[c + 3];

    // SDL is set to SDL_PIXELFORMAT_ARGB8888
    const uint32_t color = a << 24 | r << 16 | g << 8 | b;
    t.data[i] = color;
  }

  stbi_image_free(data);

  return t;
}

} // namespace render