#ifndef INC_2REAL_CAMERA_H
#define INC_2REAL_CAMERA_H

#include "../math/vec3.h"

struct Camera {
  Vec3 position;
  Vec3 rotation;
  float fov_angle;
};

#endif //INC_2REAL_CAMERA_H
