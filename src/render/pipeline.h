#pragma once
#include "camera.h"
#include "graphics.h"
#include "mesh.h"

namespace render::pipeline {

void render_entity(graphics::Context& context, const Entity& entity, const Camera& camera);

} // namespace render::pipeline
