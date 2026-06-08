#pragma once

#include "camera.h"
#include "light.h"

namespace scene {

struct Scene {
  Camera camera;
  DirectionalLight light;
};

} // namespace scene