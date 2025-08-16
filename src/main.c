#include <SDL2/SDL.h>

#define TITLE "Raycaster"
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

// 0 - false, 1 - true
#define ASSERT(code, ...) if (!code) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

int main(int argc, char* argv[]) {
    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL2 Error: ", SDL_GetError());
    SDL_Window* window = SDL_CreateWindow(TITLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
    }

    SDL_DestroyWindow(window);
    return 0;
}