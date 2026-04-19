#pragma once
#include "graphics.h"
#include "../scene/camera.h"
#include "../scene/mesh.h"

namespace render::pipeline {

void render_entity(graphics::Context& context, const scene::Entity& entity, const scene::Camera& camera);

} // namespace render::pipeline
