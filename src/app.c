#include "app.h"

struct controller* controller_init() {
    struct controller *controller = (struct controller*) malloc(sizeof(struct controller));

    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL2 Error: %s\n", SDL_GetError());

    controller->window = SDL_CreateWindow(APP_TITLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    ASSERT(controller->window, "SDL2 Error: %s\n", SDL_GetError());

    controller->renderer = SDL_CreateRenderer(controller->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    ASSERT(controller->renderer, "SDL2 Error: %s\n", SDL_GetError());

    Uint32 format = SDL_BYTEORDER == SDL_BIG_ENDIAN ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_ABGR8888;
    printf("%d\n", format);

    controller->texture = SDL_CreateTexture(controller->renderer, format, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    ASSERT(controller->texture, "SDL2 Error: %s\n", SDL_GetError());
    return controller;
}

void controller_free(struct controller* controller) {
    SDL_DestroyTexture(controller->texture);
    SDL_DestroyRenderer(controller->renderer);
    SDL_DestroyWindow(controller->window);
    free(controller);
}

void rotate(struct controller* controller, float angle) {
    float cosine    = cosf(angle);
    float sine      = sinf(angle);
    v2f d = controller->dir;
    v2f c = controller->cameraPos;

    controller->dir.x = d.x * cosine - d.y * sine;
    controller->dir.y = d.x * sine   + d.y * cosine;

    controller->cameraPos.x = c.x * cosine - c.y * sine;
    controller->cameraPos.y = c.x * sine   + c.y * cosine;
}

bool hasCollided(v2f *nxtPlayerPos, int map[], int mapSize) {
    v2i nxtPlayerBlock = (v2i) {    (int) nxtPlayerPos->x, 
                                    (int) nxtPlayerPos->y };

    return  (bool) !(nxtPlayerBlock.x >= 0 && nxtPlayerBlock.x < mapSize && 
                     nxtPlayerBlock.y >= 0 && nxtPlayerBlock.y < mapSize &&
                     map[nxtPlayerBlock.y * mapSize + nxtPlayerBlock.x] == 0);
}
