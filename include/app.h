#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "global_macros.h"
#include "utils/vector.h"

struct controller {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT]; 

    v2f playerPos, dir, cameraPos;
};

// exits if any part of the initiation fails 
struct controller* initController();

void freeController(struct controller* controller);

void rotate(struct controller* controller, float angle);

bool hasCollided(v2f *nxtPlayerPos, int map[], int mapSize);

#endif