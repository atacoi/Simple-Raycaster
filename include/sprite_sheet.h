#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "utils/vector.h"
#include "sprite.h"
#include "id.h"
#include "global_macros.h"

// assumes that each sprite has a 1 pixel gap
// including the borders
typedef struct sprite_sheet {
    SDL_Surface *spriteSheet;
    unsigned int width, height;
    Sprite *sprites;
    int spriteLength;
    v2i spriteDimensions;
} SpriteSheet;

SpriteSheet *loadSpriteSheet(char *sheetPath, SDL_Renderer *renderer, Uint32 pixelFormat, unsigned int rows, unsigned int columns);
void freeSpriteSheet(SpriteSheet *sheet);

#endif