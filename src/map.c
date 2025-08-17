#include "map.h"

void fillMap(const char *filePath, int *map[], int *mapSize) {
    FILE *fptr = fopen(filePath, "r");
    ASSERT(fptr, "%s could not be opened\n", filePath);

    char ch;
    char buffer[BUFFER_SIZE];
    char *b = buffer;
    char *end = buffer + BUFFER_SIZE;
    int line = 0;
    int mapIndex = 0;

    int *m = NULL;
    int size = 0;
    int dimensions = 0;

    fseek(fptr, -1, SEEK_END);

    int c = fgetc(fptr);

    int code = c == EOF && ferror(fptr);
    ASSERT(!code, "%s\n", "ddd");
    code = c == EOF;
    ASSERT(!code, "%s\n", "EOF");

    bool noNewLine = c == '\n';

    fseek(fptr, 0, 0);

    while((ch = (char) fgetc(fptr)) != EOF) {
        if (b == end) {
            ASSERT(0, "File Buffer exceeded memory!");
        }
        *b = ch;

        if (ch == '\n') {
            *b = '\0';
            if (line == 0) {
                size = atoi(buffer);
                *mapSize = size;
                dimensions = size * size;
                m = (int*)malloc(sizeof(int) * dimensions);
                ASSERT(m, "Memory allocation failed!");
                *map = m;
            }
            else {
                char *lst;
                long token = strtol(buffer, &lst, 10);
                char *tmp = buffer;
                while (tmp != lst) {
                    ASSERT((mapIndex < dimensions), "Map data exceeds declared size.");
                    m[mapIndex++] = (int) token;
                    while (lst != end && !isdigit(*lst) && mapIndex % size != 0) lst++;
                    tmp = lst;
                    token = strtol(tmp, &lst, 10);
                }
            }
            b = buffer;
            line++;
        } else b++;
    }

    // we have to add the final row if no newline was provided
    if (!noNewLine) {
        char *lst;
        long token = strtol(buffer, &lst, 10);
        char *tmp = buffer;
        while (tmp != lst) {
            m[mapIndex++] = (int) token;
            while (lst != end && !isdigit(*lst) && mapIndex % size != 0) lst++;
            tmp = lst;
            token = strtol(tmp, &lst, 10);
        }
    }

    fclose(fptr);
}

void freeMap(int *map[]) {
    free(*map);
    *map = NULL;
}