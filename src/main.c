#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>

#define TITLE "Raycaster"
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

#define FOV 66.0
#define PI 3.14

#define EPS 1e-5

#define MAP_SIZE 10

// 0 - false, 1 - true
#define ASSERT(code, ...) if (!(code)) { fprintf(stderr, __VA_ARGS__); }

enum Color {
    RED   = 0xFF0000FF,
    GREEN = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x00FF00FF : 0xFF00FF00,
    BLUE  = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x0000FFFF : 0xFFFF0000,
};

enum Side {
    XSIDE,
    YSIDE,
};

typedef struct vector2f {
    float x;
    float y;
} v2f;

typedef struct vector2i {
    int x;
    int y;
} v2i;

struct controller {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT]; 

    v2f playerPos, dir, cameraPos;
};


const int MAP[MAP_SIZE * MAP_SIZE] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 2, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 0, 0, 0, 1, 
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 3, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 2, 0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int min(int a, int b); 

int max(int a, int b); 
// exits if any part of the initiation fails 
void init(struct controller* controller);

void rotate(struct controller* controller, float angle);

// returns -1 if n < 0.0, 1 if n > 0.0 and 0 if n == 0.0
int sign(float n);

// Draws column x from [startY, endY]
void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]);

int hasCollided(struct controller* controller);

// assumes that pixel buffer was cleared 
void render(struct controller* controller);

int main(int argc, char* argv[]) {
    struct controller controller;

    init(&controller);

    controller.playerPos = (v2f) { 1.0f, 3.0f };
    controller.dir       = (v2f) { 0.0f, -1.0f };
    controller.cameraPos = (v2f) { 0.66f, 0.0f };

    int running = 1;
    Uint32 oldTime = 0, time = 0;
    while(running) {
        oldTime = time;
        time = SDL_GetTicks();
        float frameTime = (time - oldTime) / 1000.0;
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        float rotationAngle = 1.5f * frameTime;
        float moveSpeed = 2.0f * frameTime;

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            rotate(&controller, +rotationAngle);
        }

        if (keystate[SDL_SCANCODE_RIGHT]) {
            rotate(&controller, -rotationAngle);
        }

        if (keystate[SDL_SCANCODE_UP]) {
            controller.playerPos.x += controller.dir.x * moveSpeed;
            controller.playerPos.y += controller.dir.y * moveSpeed;
            if(!hasCollided(&controller)) {
                controller.playerPos.x -= controller.dir.x * moveSpeed;
                controller.playerPos.y -= controller.dir.y * moveSpeed;
            }
        }

        if (keystate[SDL_SCANCODE_DOWN]) {
            controller.playerPos.x -= controller.dir.x * moveSpeed;
            controller.playerPos.y -= controller.dir.y * moveSpeed;
            if(!hasCollided(&controller)) {
                controller.playerPos.x += controller.dir.x * moveSpeed;
                controller.playerPos.y += controller.dir.y * moveSpeed;
            }
        }
        printf("%f %f\n", controller.playerPos.x, controller.playerPos.y);

        memset(controller.pixels, 0, sizeof(controller.pixels));
        render(&controller);
        
        SDL_UpdateTexture(controller.texture, NULL, controller.pixels, SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderCopyEx(controller.renderer, controller.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(controller.renderer);
    }

    SDL_DestroyTexture(controller.texture);
    SDL_DestroyRenderer(controller.renderer);
    SDL_DestroyWindow(controller.window);
    return 0;
}

int min(int a, int b) {
    return a <= b ? a : b;
}

int max(int a, int b) {
    return a >= b ? a : b;
}

void init(struct controller* controller) {
    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL2 Error: %s\n", SDL_GetError());
    
    controller->window = SDL_CreateWindow(TITLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    ASSERT(controller->window, "SDL2 Error: %s\n", SDL_GetError());

    controller->renderer = SDL_CreateRenderer(controller->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    ASSERT(controller->renderer, "SDL2 Error: %s\n", SDL_GetError());

    Uint32 format = SDL_BYTEORDER == SDL_BIG_ENDIAN ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_ABGR8888;

    controller->texture = SDL_CreateTexture(controller->renderer, format, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    ASSERT(controller->texture, "SDL2 Error: %s\n", SDL_GetError());
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

int sign(float n) {
    if (fabs(n) <= EPS) return 0;

    return n < 0.0 ? -1 : 1;
}

void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]) {
    for (int y = startY; y <= endY; y++) {
        pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

int hasCollided(struct controller* controller) {
    const v2f* playerPos = &(controller->playerPos);
    v2i playerBlock      = (v2i) {  (int) playerPos->x, 
                                    (int) playerPos->y };
    return  playerBlock.x >= 0 && playerBlock.x < MAP_SIZE && 
            playerBlock.y >= 0 && playerBlock.y < MAP_SIZE &&
            MAP[playerBlock.y * MAP_SIZE + playerBlock.x] == 0;
}

void render(struct controller* controller) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // normalize x to [-1, 1]
        const float n = 2.0 * (x / (float) SCREEN_WIDTH) - 1.0;

        const v2f* playerPos    = &(controller->playerPos);
        const v2f* dir          = &(controller->dir);
        const v2f* cameraPos    = &(controller->cameraPos);

        v2f rayDir = (v2f) {    dir->x + cameraPos->x * n, 
                                dir->y + cameraPos->y * n };

        // Distance from 1 x/y block to the next x/y block, respectively 
        const v2f deltaDist = (v2f) {   fabsf(rayDir.x) <= EPS ? 1e30 : fabsf(1.0f / rayDir.x), 
                                        fabsf(rayDir.y) <= EPS ? 1e30 : fabsf(1.0f / rayDir.y) };

        v2i playerBlock = (v2i) {   (int) playerPos->x, 
                                    (int) playerPos->y };

        // Distance from playerPos to the next x/y block
        v2f sideDist = (v2f) {  (rayDir.x < 0 ? playerPos->x - playerBlock.x : playerBlock.x + 1.0f - playerPos->x) * deltaDist.x,
                                (rayDir.y < 0 ? playerPos->y - playerBlock.y : playerBlock.y + 1.0f - playerPos->y) * deltaDist.y };

        
        const int stepX = sign(rayDir.x), stepY = sign(rayDir.y); 


        // The side we hit (either a y or x side)
        int hitSide     = -1;
        Uint32 hitColor = -1;
        int hit         = 0;

        // DDA
        while (hit == 0) {
            if (sideDist.x < sideDist.y) {
                sideDist.x    += deltaDist.x;
                playerBlock.x += stepX;
                hitSide        = XSIDE;
            } else {
                sideDist.y    += deltaDist.y;
                playerBlock.y += stepY;
                hitSide        = YSIDE;
            }
            ASSERT( 0 <= playerBlock.x && playerBlock.x < MAP_SIZE &&
                    0 <= playerBlock.y && playerBlock.y < MAP_SIZE, "Out of bounds!\n");
            
            hit = MAP[(playerBlock.y * MAP_SIZE) + playerBlock.x];
        }

        switch(hit) {
            case 1:
                hitColor = RED;
                break;
            case 2:
                hitColor = GREEN;
                break;
            case 3:
                hitColor = BLUE;
                break;
        }

        // distance perpendicular to the camera plane
        float perpDist = hitSide == XSIDE ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);

        const int lineHeight  = SCREEN_HEIGHT / perpDist;
        const int bottomLine  = max(SCREEN_HEIGHT / 2 - lineHeight / 2, 0); 
        const int topLine     = min(SCREEN_HEIGHT / 2 + lineHeight / 2, SCREEN_HEIGHT - 1); 

        const Uint32 CEILING_COLOR  = 0xFFFFFFFF;
        const Uint32 FLOOR_COLOR    = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x808080FF : 0xFF808080;

        drawVertLine(x, 0, bottomLine, FLOOR_COLOR, controller->pixels);
        drawVertLine(x, bottomLine, topLine, hitColor, controller->pixels);
        drawVertLine(x, topLine, SCREEN_HEIGHT - 1, CEILING_COLOR, controller->pixels);
    }
}