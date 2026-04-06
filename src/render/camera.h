#pragma once

#include "../math/vec3.h"

struct Camera {
  Vec3 position;
  Vec3 rotation;
  float fov_angle;
};
