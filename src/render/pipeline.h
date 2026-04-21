#pragma once
#include "../scene/camera.h"
#include "../scene/light.h"
#include "../scene/mesh.h"
#include "context.h"
#include "viewport.h"

namespace render::pipeline {

void render_entity(Context& context,
                   const Viewport& viewport,
                   const scene::Entity& entity,
                   const scene::Camera& camera,
                   const scene::DirectionalLight& light);

} // namespace render::pipeline
