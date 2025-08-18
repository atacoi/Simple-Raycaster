#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>

#include "app.h"
#include "global_macros.h"
#include "utils/vector.h"
#include "map.h"
#include "sprite_sheet.h"
#include "render.h"

int main(int argc, char* argv[]) {
    struct controller* controller = initController();

    controller->playerPos = (v2f) { 2.0f, 8.0f };

    // make sure dir is facing +y axis (down)
    controller->dir = (v2f) { -1.0f, 0.0f };
    v2f_norm(&(controller->dir), EPSILON);

    controller->cameraPos = (v2f) { 0.0f, 0.66f };

    SpriteSheet *sheet = loadSpriteSheet("./tile_sheet.png", controller->renderer, SDL_PIXELFORMAT_ABGR8888, 2, 3);

    int *map = NULL;
    int mapSize = 0;
    fillMap("./file.txt", &map, &mapSize);

    int running = 1;
    Uint32 oldTime = 0, time = 0;
    v2f nxtPlayerPos = (v2f) {0.0, 0.0};
    SDL_Rect src;
    while(running) {
        oldTime = time;
        time = SDL_GetTicks();
        float frameTime = (time - oldTime) / 1000.0;
        printf("%f\n", 1.0f / frameTime);
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        float rotationAngle = 3.0f * frameTime; // radians per second
        float moveSpeed = 4.0f * frameTime; // blocks per second

        nxtPlayerPos.x = controller->playerPos.x;
        nxtPlayerPos.y = controller->playerPos.y;

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            rotate(controller, +rotationAngle);
        }

        if (keystate[SDL_SCANCODE_RIGHT]) {
            rotate(controller, -rotationAngle);
        }

        if (keystate[SDL_SCANCODE_UP]) {
            nxtPlayerPos.x += controller->dir.x * moveSpeed;
            nxtPlayerPos.y += controller->dir.y * moveSpeed;
            if(!(hasCollided(&nxtPlayerPos, map, mapSize))) {
                controller->playerPos.x = nxtPlayerPos.x;
                controller->playerPos.y = nxtPlayerPos.y;
            }
        }

        if (keystate[SDL_SCANCODE_DOWN]) {
            nxtPlayerPos.x -= controller->dir.x * moveSpeed;
            nxtPlayerPos.y -= controller->dir.y * moveSpeed;
            if(!(hasCollided(&nxtPlayerPos, map, mapSize))) {
                controller->playerPos.x = nxtPlayerPos.x;
                controller->playerPos.y = nxtPlayerPos.y;
            }
        }

        memset(controller->pixels, 0, sizeof(controller->pixels));
        render(controller, map, mapSize, sheet);
        SDL_RenderClear(controller->renderer);
        
        SDL_UpdateTexture(controller->texture, NULL, controller->pixels, SCREEN_WIDTH * sizeof(Uint32));

        SDL_RenderCopyEx(controller->renderer, controller->texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(controller->renderer);
    }

    freeMap(&map);
    freeController(controller);
    freeSpriteSheet(sheet);
    return 0;
}