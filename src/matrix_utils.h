
#ifndef MATRIX_UTILS_H_
#define MATRIX_UTILS_H_

#include <glib.h>

G_BEGIN_DECLS

typedef gdouble mat4[4][4];
typedef gdouble vec4[4];

void multiplication_mat4_vec4(vec4 result, mat4 m, vec4 v);
void multiplication_vec4_vec4(double *result, vec4 v1, vec4 v2);

G_END_DECLS


#endif /* MATRIX_UTILS_H_ */
