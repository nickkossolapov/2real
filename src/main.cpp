#include <SDL3/SDL.h>
#include <cstdio>

int main(int argc, char* argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("2real", 640, 480, 0);

  if (!window) {
    printf("Window Error: %s\n", SDL_GetError());
    SDL_Quit();

    return 1;
  }

  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }

      if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.key == SDLK_ESCAPE) {
          quit = true;
        }
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
