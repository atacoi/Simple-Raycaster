#include "map.h"

int main() {
    int *map = NULL;
    int size = 0;
    fillMap("./file.txt", &map, &size);
    freeMap(&map);
}