#include "sprite_sheet.h"
 
SpriteSheet *loadSpriteSheet(char *sheetPath, SDL_Renderer *renderer, Uint32 pixelFormat, unsigned int rows, unsigned int columns) {
    SDL_Surface *original = IMG_Load(sheetPath);
    ASSERT(original, "IMG_Load failed: %s\n", IMG_GetError());

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(original, pixelFormat, 0);
    SDL_FreeSurface(original);
    ASSERT(surface, "Image Failed to Load: %s\n", SDL_GetError());

    SpriteSheet *sheet = (SpriteSheet*) malloc(sizeof(SpriteSheet));
    sheet->width = surface->w;
    sheet->height = surface->h;
    sheet->spriteSheet = surface;

    int spriteCount = rows * columns;
    sheet->sprites = (Sprite*)malloc(sizeof(Sprite) * spriteCount);
    sheet->spriteDimensions = (v2i) {   (sheet->width - (columns + 1)) / columns, 
                                        (sheet->height - (rows + 1)) / rows };
    sheet->spriteLength = spriteCount;
    int posX = 1, posY = 1;
    int index = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            sheet->sprites[index] = (Sprite) { getNextId(), (v2i) {posX, posY} };
            posX += sheet->spriteDimensions.x + 1;
            index++;
        }
        posX = 1;
        posY += sheet->spriteDimensions.y + 1;
    }
    return sheet;
}

void freeSpriteSheet(SpriteSheet *sheet) {
    // unsigned length = sheet->spriteLen;
    // Sprite *sprites = sheet->sprites;
    // Sprite *s = sprites;
    // Sprite *endSheet = sprites + length;

    // while (s != endSheet) {
    //     freeSprite(*s);
    //     s++;
    // }

    free(sheet->sprites);
    SDL_FreeSurface(sheet->spriteSheet);
    free(sheet);
}