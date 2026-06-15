#include "engine/fps_logger.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "engine/timing.h"
#include "input/input.h"
#include "math/common.h"
#include "math/vec3.h"
#include "render/color.h"
#include "render/framebuffer.h"
#include "render/obj_loader.h"
#include "render/renderer.h"
#include "render/texture_loader.h"
#include "scene/camera.h"
#include "scene/camera_controller.h"
#include "scene/mesh.h"

#include <algorithm>
#include <format>

namespace {

std::optional<scene::Entity> create_entity(const std::string& name) {
  const auto mesh = render::load_obj_file(std::format("./assets/{0}.obj", name));
  const auto texture = render::load_texture_file(std::format("./assets/{0}.png", name));

  if (!mesh.has_value()) {
    SDL_Log("Failed to load mesh for %s", name.data());

    return {};
  }

  scene::Entity entity = {.mesh = std::make_shared<scene::Mesh>(std::move(*mesh))};

  if (!texture.has_value()) {
    SDL_Log("Failed to load texture for %s", name.data());

    return {};
  }

  entity.texture = std::make_shared<render::Texture>(texture.value());
  entity.flat_colour = render::color::white;

  return entity;
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings = {
      .width = 320,
      .height = 240,
      .scale = 4,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {.sdl_settings = settings};

  scene::Scene scene = {
      .camera = {.fov = math::deg_to_rad(60.0f),
                 .aspect_ratio = static_cast<float>(settings.width) / static_cast<float>(settings.height),
                 .z_near = 0.1f,
                 .z_far = 100.0f,
                 .position = {0.0f, 0.0f, 0.0f}},
      .light = scene::DirectionalLight({-0.5f, -1.0f, 0.5f}),
  };

  auto f22 = create_entity("f22").value();
  auto f117 = create_entity("f117").value();
  auto efa = create_entity("efa").value();
  auto runway = create_entity("runway").value();

  scene.camera.position.x = 1;
  scene.camera.position.y = 2;
  scene.camera.position.z = 6;
  scene.camera.rotation.y = math::deg_to_rad(170);
  scene.camera.rotation.x = math::deg_to_rad(-20);

  f22.transform.position.z = 0;
  f22.transform.rotation.y = math::deg_to_rad(90);

  f117.transform.position.z = 1;
  f117.transform.position.x = 2;
  f117.transform.rotation.y = math::deg_to_rad(120);

  efa.transform.position.z = 1;
  efa.transform.position.x = -2;
  efa.transform.rotation.y = math::deg_to_rad(60);

  runway.transform.position.y = -0.35;

  std::vector entities{f22, f117, efa, runway};

  auto update = [&scene](float, const input::Snapshot& input) { update_fps_camera(scene.camera, input); };

  auto render = [&entities, &scene](render::Framebuffer& framebuffer) {
    for (auto& entity : entities) {
      render::render_entity(framebuffer, scene, entity, render::RenderMode::Textured);
    }
  };

  return engine::run(app_config, update, render);
}
