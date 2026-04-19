#pragma once
#include "graphics.h"
#include "viewport.h"
#include "../scene/camera.h"
#include "../scene/light.h"
#include "../scene/mesh.h"

namespace render::pipeline {

void render_entity(graphics::Context& context,
                   const Viewport& viewport,
                   const scene::Entity& entity,
                   const scene::Camera& camera,
                   const scene::DirectionalLight& light);

} // namespace render::pipeline
