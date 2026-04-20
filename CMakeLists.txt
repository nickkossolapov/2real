cmake_minimum_required(VERSION 3.21...4.3)
project(2real)

find_package(SDL3 CONFIG REQUIRED)

add_library(
        2real-engine STATIC
        src/engine/sdl.cpp
        src/engine/sdl.h
        src/engine/timing.h
        src/input/input.cpp
        src/input/input.h
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
        src/render/viewport.h
        src/scene/camera.h
        src/scene/light.h
        src/scene/mesh.h
)

target_compile_features(2real-engine PUBLIC cxx_std_20)
target_link_libraries(2real-engine PUBLIC SDL3::SDL3)
target_include_directories(2real-engine PUBLIC src)

function(add_demo name folder include_assets)
    add_executable(${name} demos/${folder}/${folder}.cpp)
    target_link_libraries(${name} PRIVATE 2real-engine)
    if(include_assets)
        add_custom_command(TARGET ${name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:${name}>/${folder}/assets
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/demos/${folder}/assets $<TARGET_FILE_DIR:${name}>/${folder}/assets)
    endif()
endfunction()

add_demo(basic-rendering basic_rendering TRUE)
