#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>

#include "app.h"
#include "global_macros.h"
#include "utils/vector.h"
#include "map.h"

enum Color {
    RED   = 0xFF0000FF,
    GREEN = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x00FF00FF : 0xFF00FF00,
    BLUE  = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x0000FFFF : 0xFFFF0000,
};

enum Side {
    XSIDE,
    YSIDE,
};

struct enemy {
    v2f pos;
    SDL_Surface* img_surface;
} tenna;

// Draws column x from [startY, endY]
void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]);

// assumes that pixel buffer was cleared 
void render(struct controller* controller, int map[], int mapSize);

int main(int argc, char* argv[]) {
    struct controller* controller = controller_init();

    controller->playerPos.x = 1.0f;
    controller->playerPos.y = 3.0f;

    // make sure dir is facing +y axis (down)
    controller->dir.x = -1.0f;
    controller->dir.y = 0.0f;
    v2f_norm(&(controller->dir), EPSILON);

    controller->cameraPos.x = 0.0f;
    controller->cameraPos.y = 0.66f;

    SDL_Surface *original = IMG_Load("./imgs/test.png");
    ASSERT(original, "IMG_Load failed: %s\n", IMG_GetError());

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(original, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(original);  // free original after conversion
    tenna.img_surface = surface;
SDL_PixelFormat *fmt = tenna.img_surface->format;

    ASSERT((tenna.img_surface), "Image Failed to Load: %s\n", SDL_GetError());

    tenna.pos = (v2f) {1.0f, 1.0f};

    int *map = NULL;
    int mapSize = 0;
    fillMap("./file.txt", &map, &mapSize);

    int running = 1;
    Uint32 oldTime = 0, time = 0;
    v2f nxtPlayerPos = (v2f) {0.0, 0.0};
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
        render(controller, map, mapSize);
        SDL_RenderClear(controller->renderer);
        
        SDL_UpdateTexture(controller->texture, NULL, controller->pixels, SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderCopyEx(controller->renderer, controller->texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(controller->renderer);
    }
    return 0;
}

void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]) {
    for (int y = startY; y <= endY; y++) {
        pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

void render(struct controller* controller, int map[], int mapSize) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // normalize x to [-1, 1]
        const float n = 2.0 * (x / (float) SCREEN_WIDTH) - 1.0;

        const v2f* playerPos    = &(controller->playerPos);
        const v2f* dir          = &(controller->dir);
        const v2f* cameraPos    = &(controller->cameraPos);

        v2f rayDir = (v2f) {    dir->x + cameraPos->x * n, 
                                dir->y + cameraPos->y * n };

        // Distance from 1 x/y block to the next x/y block, respectively 
        const v2f deltaDist = (v2f) {   fabsf(rayDir.x) <= EPSILON ? 1e30 : fabsf(1.0f / rayDir.x), 
                                        fabsf(rayDir.y) <= EPSILON ? 1e30 : fabsf(1.0f / rayDir.y) };

        v2i playerBlock = (v2i) {   (int) playerPos->x, 
                                    (int) playerPos->y };

        // Distance from playerPos to the next x/y block
        v2f sideDist = (v2f) {  (rayDir.x < 0 ? playerPos->x - playerBlock.x : playerBlock.x + 1.0f - playerPos->x) * deltaDist.x,
                                (rayDir.y < 0 ? playerPos->y - playerBlock.y : playerBlock.y + 1.0f - playerPos->y) * deltaDist.y };

        
        const int stepX = SIGN(rayDir.x), stepY = SIGN(rayDir.y); 


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
            ASSERT( 0 <= playerBlock.x && playerBlock.x < mapSize &&
                    0 <= playerBlock.y && playerBlock.y < mapSize, "Out of bounds!\n");
            
            hit = map[(playerBlock.y * mapSize) + playerBlock.x];
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
        
        int texNum = hit - 1;
        if (texNum == 1) {
            ASSERT(SDL_LockSurface(tenna.img_surface), "SDL2 Error: fuck %s\n", SDL_GetError());

            int texWidth  = tenna.img_surface->w;   // 121 in your case
            int texHeight = tenna.img_surface->h;

            int lineHeight  = (int) (SCREEN_HEIGHT / perpDist);
            int bottomLine  = (int) MAX(SCREEN_HEIGHT / 2 - lineHeight / 2, 0);
            int topLine     = (int) MIN(SCREEN_HEIGHT / 2 + lineHeight / 2, SCREEN_HEIGHT - 1);

            const Uint32 CEILING_COLOR  = 0xFFFFFFFF;
            const Uint32 FLOOR_COLOR    = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x808080FF : 0xFF808080;

            drawVertLine(x, 0, bottomLine, FLOOR_COLOR, controller->pixels);
            drawVertLine(x, topLine, SCREEN_HEIGHT - 1, CEILING_COLOR, controller->pixels);

            // compute exact hit coordinate along wall
            float wallX;
            if (hitSide == XSIDE) wallX = playerPos->y + perpDist * rayDir.y;
            else           wallX = playerPos->x + perpDist * rayDir.x;
            wallX -= floor(wallX);

            int texX = (int)(wallX * texWidth);
            // correct the offset so that the image is flipped vertically 
            // https://permadi.com/tutorial/raycast/rayc10.html
            if (hitSide == XSIDE && rayDir.x > 0) texX = texWidth - texX - 1;
            if (hitSide == YSIDE && rayDir.y < 0) texX = texWidth - texX - 1;

            // vertical stepping for the texture in terms of the lineHeight
            double step   = 1.0 * texHeight / lineHeight;
            // dealing with edge case when there is no ceiling visible so flip 
            // the bottom so that it falls on the textured wall's line
            double texPos =  (bottomLine - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;

            int pitch = tenna.img_surface->pitch;

            printf("Image format: %s\n", SDL_GetPixelFormatName(tenna.img_surface->format->format));

            // iterate from top to bottom since final orientation is flipped 
            for (int y = topLine - 1; y >= bottomLine; y--) {
                int texY = ((int) texPos) % texHeight;
                Uint32* const target_pixel = (Uint32 *) ((Uint8*) tenna.img_surface->pixels + texY * tenna.img_surface->pitch + texX * tenna.img_surface->format->BytesPerPixel);

                controller->pixels[y * SCREEN_WIDTH + x] =
                    *target_pixel;

                texPos += step;
            }
            SDL_UnlockSurface(tenna.img_surface);
        }

        else {
            int lineHeight  = (int) (SCREEN_HEIGHT / perpDist);
            int bottomLine  = (int) MAX(SCREEN_HEIGHT / 2 - lineHeight / 2, 0);
            int topLine     = (int) MIN(SCREEN_HEIGHT / 2 + lineHeight / 2, SCREEN_HEIGHT - 1);

            const Uint32 CEILING_COLOR  = 0xFFFFFFFF;
            const Uint32 FLOOR_COLOR    = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x808080FF : 0xFF808080;

            drawVertLine(x, 0, bottomLine, FLOOR_COLOR, controller->pixels);
            drawVertLine(x, bottomLine, topLine, hitColor, controller->pixels);
            drawVertLine(x, topLine, SCREEN_HEIGHT - 1, CEILING_COLOR, controller->pixels);
        }
    }
}