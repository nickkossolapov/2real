cmake_minimum_required(VERSION 3.21...4.3)
project(2real)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(
        2real
        src/engine/init.h
        src/engine/timing.h
        src/input/input.cpp
        src/input/input.h
        src/main.cpp
        src/math/common.h
        src/math/mat4.h
        src/math/vec2.h
        src/math/vec3.h
        src/math/vec4.h
        src/render/draw.cpp
        src/render/draw.h
        src/render/graphics.cpp
        src/render/obj_loader.cpp
        src/render/obj_loader.h
        src/render/pipeline.cpp
        src/render/pipeline.h
        src/render/triangle.h
        src/scene/camera.h
        src/scene/light.h
        src/scene/mesh.h
)

target_link_libraries(2real PRIVATE SDL3::SDL3)

add_custom_command(TARGET 2real POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:2real>/assets)
