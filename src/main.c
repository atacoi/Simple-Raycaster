#include <SDL2/SDL.h>
#include <stdlib.h>

#define TITLE "Raycaster"
#define SCREEN_WIDTH 200
#define SCREEN_HEIGHT 200

enum Color {
    RED   = 0xFF0000FF,
    GREEN = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x00FF00FF : 0xFF00FF00,
    BLUE  = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x0000FFFF : 0xFFFF0000,
};

// 0 - false, 1 - true
#define ASSERT(code, ...) if (!code) { fprintf(stderr, __VA_ARGS__); }

struct controller {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT]; 
} controller;

int main(int argc, char* argv[]) {
    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL2 Error: ", SDL_GetError());
    
    controller.window = SDL_CreateWindow(TITLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    ASSERT(controller.window, "SDL2 Error: ", SDL_GetError());

    controller.renderer = SDL_CreateRenderer(controller.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    ASSERT(controller.renderer, "SDL2 Error: ", SDL_GetError());

    Uint32 format = SDL_BYTEORDER == SDL_BIG_ENDIAN ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_ABGR8888;

    controller.texture = SDL_CreateTexture(controller.renderer, format, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    ASSERT(controller.texture, "SDL2 Error: ", SDL_GetError());

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        memset(controller.pixels, 0, sizeof(controller.pixels));
        for (int w = 0; w < SCREEN_WIDTH; w++) {
            controller.pixels[(SCREEN_HEIGHT / 2 - 1) * SCREEN_WIDTH + w] = GREEN;
        }

        SDL_UpdateTexture(controller.texture, NULL, controller.pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(controller.renderer, controller.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(controller.renderer);
    }

    SDL_DestroyTexture(controller.texture);
    SDL_DestroyRenderer(controller.renderer);
    SDL_DestroyWindow(controller.window);
    return 0;
}