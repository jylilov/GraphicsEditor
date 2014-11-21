#include "matrix_utils.h"

void multiplication_mat4_vec4(vec4 result, mat4 m, vec4 v) {
    int i, j;

    for (i = 0; i < 4; ++i) {
        result[i] = 0;
        for (j = 0; j < 4; ++j) {
            result[i] += m[i][j] * v[j];
        }
    }
}

void multiplication_vec4_vec4(double *result, vec4 v1, vec4 v2) {
    int i;
    *result = 0;
    for (i = 0; i < 4; ++i) {
        *result += v1[i] * v2[i];
    }
}
