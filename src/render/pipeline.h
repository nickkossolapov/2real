#pragma once
#include "context.h"
#include "scene/camera.h"
#include "scene/light.h"
#include "scene/mesh.h"
#include "viewport.h"

namespace render {

enum class RenderMode {
  Textured,
  Flat,
  Wireframe
};

namespace pipeline {
void render_entity(Context& context,
                   const Viewport& viewport,
                   const scene::Entity& entity,
                   const scene::Camera& camera,
                   const scene::DirectionalLight& light,
                   RenderMode mode);
} // namespace  pipeline

} // namespace render
