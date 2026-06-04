# Physics Debug View

The game renders at 320×240 for that PS1 look, but jitter, interpenetration, constraint drift will disappear into pixel noise.

## Approach

SDL's renderer already operates at 1280×960 (the window size). After blitting the game texture but before `SDL_RenderPresent`, any `SDL_RenderLine` / `SDL_RenderRect` calls draw at full native resolution on top of the upscaled frame. No extra framebuffer needed.

Split `Context::present()` into two steps:

```cpp
void upload_frame(SDL_Renderer&, SDL_Texture&); // clear, upload 320×240 texture, blit
void finish_frame(SDL_Renderer&); // SDL_RenderPresent + buffer clear
```

Then in the game loop:

```cpp
renderer.upload_frame(sdl.renderer(), sdl.display_texture());
debug_draw_physics(sdl.renderer());  // full res overlay
renderer.finish_frame(sdl.renderer());
```

## Projection

Debug wireframes still need to go through view > projection > viewport transforms to land in the right place. Reuse the existing `project()` math, then scale screen coordinates by `window::scale` (4×) so they map to 1280×960 instead of 320×240.

```cpp
auto screen = project(viewport, projection, world_pos);
float x = screen.pos.x * engine::window::scale;
float y = screen.pos.y * engine::window::scale;
```

No clipping, depth buffer, or scanline fill needed — just project → scale → SDL draw.

## What to draw

- Collision wireframes (AABB, convex hull outlines)
- Contact points and normals
- Constraint anchors and limits
- Numerical readouts: penetration depth, energy drift, solver residuals

# ImGui

https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdlrenderer3.h
