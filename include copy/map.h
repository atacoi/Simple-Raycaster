#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "global_macros.h"

// done in-place
void fillMap(const char *filePath, int *map[], int *mapSize);

void freeMap(int *map[]);

#endif