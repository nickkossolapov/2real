#pragma once

#include "framebuffer.h"
#include "scene/mesh.h"
#include "scene/scene.h"

namespace render {

enum class RenderMode {
  Textured,
  Flat,
  Wireframe
};

void render_entity(Framebuffer& framebuffer, const scene::Scene& scn, const scene::Entity& entity, RenderMode mode);

} // namespace render
