#ifndef RENDER_H
#define RENDER_H

#include "app.h"
#include "sprite_sheet.h"
#include <SDL2/SDL.h>
#include "global_macros.h"

// Draws column x from [startY, endY]
void drawVertLine(int x, int startY, int endY, Uint32 color, Uint32 pixels[]);

// assumes that pixel buffer was cleared 
void render(struct controller* controller, int map[], int mapSize, SpriteSheet *sheet);

#endif 