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
        src/render/triangle.h
        src/render/draw.h
        src/render/draw.cpp)
target_link_libraries(2real PRIVATE SDL3::SDL3)
