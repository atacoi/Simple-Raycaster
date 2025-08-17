#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <math.h>
#include <stdio.h>

typedef struct vector2f {
    float x;
    float y;
} v2f;

typedef struct vector2i {
    int x;
    int y;
} v2i;

/* **** vector2i functions **** */

// returns the k component's magnitude 
int v2i_cross(v2i *v1, v2i *v2);

int v2i_dot(v2i *v1, v2i *v2);

void v2i_print(v2i *v);

/* **** vector2f functions **** */

float v2f_cross(v2f *v1, v2f *v2);

float v2f_dot(v2f *v1, v2f *v2);

float v2f_length(v2f *v);

// in-place normalization
// epsilon used for checking whether the values are close enough to 0.0
bool v2f_norm(v2f *v, float epsilon);

void v2f_print(v2f *v);

#endif