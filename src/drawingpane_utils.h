#ifndef __DRAWING_PANE_UTILS_H
#define __DRAWING_PANE_UTILS_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _Pixel Pixel;
typedef struct _Point Point;

struct _Pixel {
	gint x, y;
	gdouble alpha;
};

struct _Point {
	gint x, y;
};

GList *get_dda_line_figure(gint x1, gint y1, gint x2, gint y2);
GList *get_bresenham_line_figure(gint x1, gint y1, gint x2, gint y2);
GList *get_wu_line_figure(gint x1, gint y1, gint x2, gint y2);
GList *get_hyperbole_figure(gint a, gint b, gint x0, gint y0, gint width, gint height);
GList *get_b_spline_figure(GList *points, gdouble step);

G_END_DECLS

#endif /* __DRAWING_PANE_UTILS_H */
