cmake_minimum_required(VERSION 3.21...4.3)
project(2real LANGUAGES CXX)

# Standardize output dirs between Linux, macOS, and Windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(SDL3 CONFIG REQUIRED)

add_library(
        2real-engine STATIC
        src/engine/fps_logger.h
        src/engine/run.cpp
        src/engine/run.h
        src/engine/sdl.cpp
        src/engine/sdl.h
        src/engine/timing.h
        src/input/input.cpp
        src/input/input.h
        src/math/common.h
        src/math/mat4.h
        src/math/plane.h
        src/math/rect.h
        src/math/vec2.h
        src/math/vec3.h
        src/math/vec4.h
        src/physics/particle.cpp
        src/physics/particle.h
        src/render/clipping.cpp
        src/render/clipping.h
        src/render/color.h
        src/render/draw.cpp
        src/render/draw.h
        src/render/framebuffer.cpp
        src/render/framebuffer.h
        src/render/frustum.h
        src/render/obj_loader.cpp
        src/render/obj_loader.h
        src/render/renderer.cpp
        src/render/renderer.h
        src/render/texture.h
        src/render/texture_loader.cpp
        src/render/texture_loader.h
        src/render/triangle.h
        src/scene/camera.h
        src/scene/camera_controller.h
        src/scene/light.h
        src/scene/mesh.h
        src/scene/scene.h
        src/physics/force.h
        src/physics/force.cpp
)

target_compile_features(2real-engine PUBLIC cxx_std_20)
target_link_libraries(2real-engine PUBLIC SDL3::SDL3)
target_include_directories(2real-engine PUBLIC src third_party)

function(setup_demo name folder include_assets)
    target_link_libraries(${name} PRIVATE 2real-engine)

    set_target_properties(${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}")

    if (include_assets AND EXISTS "${CMAKE_SOURCE_DIR}/demos/${folder}/assets")
        add_custom_command(TARGET ${name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:${name}>/assets"
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/demos/${folder}/assets"
                "$<TARGET_FILE_DIR:${name}>/assets"
                COMMENT "Freshly syncing assets for ${name}"
                VERBATIM)
    endif ()
endfunction()

add_executable(basic-rendering
        demos/basic_rendering/basic_rendering.cpp
)

setup_demo(basic-rendering basic_rendering TRUE)

add_executable(physics_2d
        demos/physics_2d/physics_2d.cpp
)

setup_demo(physics_2d physics_2d TRUE)
