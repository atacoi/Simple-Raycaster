#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

#define APP_TITLE "Raycaster"
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

// 0 - false, 1 - true
#define ASSERT(code, ...) if (!(code)) { fprintf(stderr, __VA_ARGS__); }

#define MAX(a, b) ({        \
        typeof (a) _a = (a); \
        typeof (b) _b = (b); \
        a > b ? a : b; })

#define MIN(a, b) ({        \
        typeof (a) _a = (a); \
        typeof (b) _b = (b); \
        a < b ? a : b; })   

#define SIGN(a) a > 0 ? 1 : -1

#define FOV 66.0
#define PI 3.14

#define EPSILON 1e-5

#define BUFFER_SIZE 1080

#define XSIDE 0
#define YSIDE 1

#endif