#include "render.h"

enum Color {
    RED   = 0xFF0000FF,
    GREEN = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x00FF00FF : 0xFF00FF00,
    BLUE  = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x0000FFFF : 0xFFFF0000,
};

void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]) {
    for (int y = startY; y <= endY; y++) {
        pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

void render(struct controller* controller, int map[], int mapSize, SpriteSheet *sheet) {
    SDL_Surface *spriteSheet = sheet->spriteSheet;

    const v2f* playerPos    = &(controller->playerPos);
    const v2f* dir          = &(controller->dir);
    const v2f* cameraPos    = &(controller->cameraPos);

    const Uint32 CEILING_COLOR  = 0xFFFFFFFF;
    const Uint32 FLOOR_COLOR    = SDL_BYTEORDER == SDL_BIG_ENDIAN ? 0x808080FF : 0xFF808080;

    ASSERT((!SDL_LockSurface(spriteSheet)), "SDL2 Error: %s\n", SDL_GetError());

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // normalize x to [-1, 1]
        const float n = 2.0 * (x / (float) SCREEN_WIDTH) - 1.0;

        v2i mapCoord = (v2i) {  (int) playerPos->x, 
                                (int) playerPos->y };

        v2f rayDir = (v2f) {    dir->x + cameraPos->x * n, 
                                dir->y + cameraPos->y * n };

        // Distance from 1 x/y block to the next x/y block, respectively 
        const v2f deltaDist = (v2f) {   fabsf(rayDir.x) <= EPSILON ? 1e30 : fabsf(1.0f / rayDir.x), 
                                        fabsf(rayDir.y) <= EPSILON ? 1e30 : fabsf(1.0f / rayDir.y) };


        // Distance from playerPos to the next x/y block
        v2f sideDist = (v2f) {  (rayDir.x < 0 ? playerPos->x - mapCoord.x : mapCoord.x + 1.0f - playerPos->x) * deltaDist.x,
                                (rayDir.y < 0 ? playerPos->y - mapCoord.y : mapCoord.y + 1.0f - playerPos->y) * deltaDist.y };

        
        const int stepX = SIGN(rayDir.x), stepY = SIGN(rayDir.y); 


        // The side we hit (either a y or x side)
        int hitSide     = -1;
        int hit         = 0;

        // DDA
        while (hit == 0) {
            if (sideDist.x < sideDist.y) {
                sideDist.x    += deltaDist.x;
                mapCoord.x += stepX;
                hitSide        = XSIDE;
            } else {
                sideDist.y    += deltaDist.y;
                mapCoord.y += stepY;
                hitSide        = YSIDE;
            }
            ASSERT( 0 <= mapCoord.x && mapCoord.x < mapSize &&
                    0 <= mapCoord.y && mapCoord.y < mapSize, "Out of bounds!\n");
            
            hit = map[(mapCoord.y * mapSize) + mapCoord.x];
        }

        // distance perpendicular to the camera plane
        float perpDist = hitSide == XSIDE ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
        
        int texNum = hit - 1;
        ASSERT((texNum < sheet->spriteLength && texNum >= 0), "Texture index out of bounds: %d\n", texNum);

        Sprite *hitSprite = &(sheet->sprites[texNum]);

        int texWidth  = sheet->spriteDimensions.x;  
        int texHeight = sheet->spriteDimensions.y;

        int lineHeight  = (int) (SCREEN_HEIGHT / perpDist);
        int bottomLine  = (int) MAX(SCREEN_HEIGHT / 2 - lineHeight / 2, 0);
        int topLine     = (int) MIN(SCREEN_HEIGHT / 2 + lineHeight / 2, SCREEN_HEIGHT - 1);

        // draw floor and ceiling 
        drawVertLine(x, 0, bottomLine, FLOOR_COLOR, controller->pixels);
        drawVertLine(x, topLine, SCREEN_HEIGHT - 1, CEILING_COLOR, controller->pixels);

        // compute exact hit coordinate along wall
        float wallX;
        if (hitSide == XSIDE)   wallX = playerPos->y + perpDist * rayDir.y;
        else                    wallX = playerPos->x + perpDist * rayDir.x;
        wallX -= floor(wallX);

        int texX = (int)(wallX * texWidth);
        // correct the offset so that the image is flipped vertically 
        // https://permadi.com/tutorial/raycast/rayc10.html
        if (hitSide == XSIDE && rayDir.x > 0) texX = texWidth - texX - 1;
        if (hitSide == YSIDE && rayDir.y < 0) texX = texWidth - texX - 1;

        texX += hitSprite->sideStart.x;

        // vertical stepping for the texture in terms of the lineHeight
        double step   = 1.0 * texHeight / lineHeight;
        // dealing with edge case when there is no ceiling visible so flip 
        // the bottom so that it falls on the textured wall's line
        double texPos =  (bottomLine - SCREEN_HEIGHT / 2 + lineHeight / 2) * step + hitSprite->sideStart.y;

        int pitch = spriteSheet->pitch;

        // iterate from top to bottom since final orientation is flipped 
        for (int y = topLine - 1; y >= bottomLine; y--) {
            int texY = ((int) texPos);
            Uint32* const target_pixel = (Uint32 *) ((Uint8*) spriteSheet->pixels + texY * spriteSheet->pitch + texX * spriteSheet->format->BytesPerPixel);

            controller->pixels[y * SCREEN_WIDTH + x] =
                *target_pixel;

            texPos += step;
        }
    }
    SDL_UnlockSurface(spriteSheet);
}