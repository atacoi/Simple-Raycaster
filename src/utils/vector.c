#include "utils/vector.h"

/* **** general macros **** */

#define CROSS(v1, v2) ({        \
        typeof (v1) _v1 = (v1); \
        typeof (v2) _v2 = (v2); \
        _v1->x * _v2->y - _v1->y * _v2->x; })

#define DOT(v1, v2) ({        \
        typeof (v1) _v1 = (v1); \
        typeof (v2) _v2 = (v2); \
        _v1->x * _v2->x + _v1->y * _v2->y; })

#define LEN(v) ({        \
        typeof (v) _v = (v); \
        DOT(v, v); })

#define NORM(v) ({        \
        typeof (v) _v = (v); \
        typeof (LEN(v)) _l = LEN(v); \
        v->x /= LEN; \
        v->y /= LEN;})

/* **** vector2i functions **** */

int v2i_cross(v2i *v1, v2i *v2) { return CROSS(v1, v2); }

int v2i_dot(v2i *v1, v2i *v2) { return DOT(v1, v2); }

void v2i_print(v2i *v) {
    printf("Vector2i {%d, %d}\n", v->x, v->y);
}

/* **** vector2f functions **** */

float v2f_cross(v2f *v1, v2f *v2) { return CROSS(v1, v2); }

float v2f_dot(v2f *v1, v2f *v2) { return DOT(v1, v2); }

float v2f_length(v2f *v) { 
    return sqrtf(v2f_dot(v, v));
}

bool v2f_norm(v2f *v, float epsilon) {
    if (fabsf(v->x) <= epsilon && fabsf(v->y) <= epsilon) return false;

    const float len = v2f_length(v);
    v->x /= len;
    v->y /= len;
    return true;
}

void v2f_print(v2f *v) {
    printf("Vector2f {%f, %f}\n", v->x, v->y);
}