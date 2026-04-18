cmake_minimum_required(VERSION 3.21...4.3)
project(2real)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(2real
        src/main.cpp
        src/render/graphics.cpp
        src/math/vec2.h
        src/math/vec3.h
        src/render/camera.h
        src/input.h
        src/engine/timing.h
        src/render/draw.h
        src/render/draw.cpp
        src/render/mesh.h
        src/render/obj_loader.h
        src/render/obj_loader.cpp
        src/math/mat4.h
        src/math/vec4.h
        src/render/triangle.h)
target_link_libraries(2real PRIVATE SDL3::SDL3)

add_custom_command(TARGET 2real POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:2real>/assets)
