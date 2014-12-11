#include "drawingpane_utils.h"
#include "matrix_utils.h"
#include <math.h>

#define SQR(A) (A) * (A)

static gint sign(gdouble x);
static void swap(gint *a, gint *b);
static void add_pixel(GList **figure, gint x, gint y);
static void add_pixel_with_alpha(GList **figure, gint x, gint y, gdouble alpha);
static gboolean add_pixel_in_zone(GList **figure, gint x, gint y, gint x0, gint y0, gint width, gint height);

static
gint sign(gdouble x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

static void
add_pixel_with_alpha(GList **figure, gint x, gint y, gdouble alpha) {
	Pixel *pixel;

	pixel = g_malloc(sizeof(Pixel));

	pixel->x = x;
	pixel->y = y;
	pixel->alpha = alpha;

	*figure = g_list_append(*figure, pixel);
}

static void
add_pixel(GList **figure, gint x, gint y) {
	add_pixel_with_alpha(figure, x, y, 1);
}

GList *
get_dda_line_figure(gint x1, gint y1, gint x2, gint y2) {
	GList *figure;
	gint length;
	gdouble dx, dy;
	gdouble x, y;

	figure = NULL;
	length = MAX(abs(x2 - x1), abs(y2 - y1));

	dx = (x2 - x1) / (gfloat)length;
	dy = (y2 - y1) / (gfloat)length;

	x = x1;
	y = y1;

	gint i;
	for (i = 0; i <= length; ++i) {
		add_pixel(&figure, round(x), round(y));
		x += dx;
		y += dy;
	}

	return figure;
}

static void
swap(gint *a, gint *b) {
	gint o = *a;
	*a = *b;
	*b = o;
}

GList *
get_bresenham_line_figure(gint x1, gint y1, gint x2, gint y2) {
	GList *figure;
	gint dx, dy;
	gint e, x, y;
	gboolean is_swaped = FALSE;

	figure = NULL;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	if (dx > dy) {
		if (x1 > x2) {
			is_swaped = TRUE;
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		gint inc_y = sign(y2 - y1);
		e = dy;

		for (x = x1, y = y1; x <= x2; ++x) {
			add_pixel(&figure, x, y);

			if (2 * e >= dx) {
				y += inc_y;
				e -= dx;
			}

			e += dy;
		}
	} else {
		if (y1 > y2) {
			is_swaped = TRUE;
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		gint inc_x = sign(x2 - x1);
		e = dx;

		for (x = x1, y = y1; y <= y2 + 0.5; ++y) {
			add_pixel(&figure, x, y);

			if (2 * e >= dy) {
				x += inc_x;
				e -= dy;
			}

			e += dx;
		}
	}

	if (is_swaped == TRUE) {
		figure = g_list_reverse(figure);
	}

	return figure;
}

GList *
get_wu_line_figure(gint x1, gint y1, gint x2, gint y2) {
	GList *figure;
	gint dx, dy, steps;
	gdouble e, de;
	gint x, y;
	int i;

	gint d_second_y;
	gint d_second_x;

	gint step_inc_x;
	gint step_inc_y;

	figure = NULL;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	if (dx == 0 || dy == 0 || dx == dy) {
		return get_bresenham_line_figure(x1, y1, x2, y2);
	}

	if (dx > dy) {
		step_inc_x = x1 > x2 ? -1 : 1;
		step_inc_y = 0;
		d_second_x = 0;
		d_second_y = y1 > y2 ? -1 : 1;
		steps = dx;
		de = (gdouble) dy / dx;
	} else {
		step_inc_x = 0;
		step_inc_y = y1 > y2 ? -1 : 1;
		d_second_x = x1 > x2 ? -1 : 1;
		d_second_y = 0;
		steps = dy;
		de = (gdouble) dx / dy;
	}

	e = 0;

	x = x1; y = y1;

	for (i = 0; i <= steps; ++i) {
		add_pixel_with_alpha(&figure, x, y, 1 - e);
		add_pixel_with_alpha(&figure, x + d_second_x, y + d_second_y, e);

		e += de;

		if (e > 1) {
			x += d_second_x;
			y += d_second_y;
			e -= 1;
		}

		x += step_inc_x;
		y += step_inc_y;
	}

	return figure;
}

static gboolean
add_pixel_in_zone(GList **figure, gint x, gint y, gint x0, gint y0, gint width, gint height) {
	if (x >= x0 && x <= x0 + width &&
			y >= y0 && y <= y0 + height) {
		add_pixel(figure, x, y);
		return TRUE;
	} else {
		return FALSE;
	}
}

GList *get_hyperbole_figure(gint a, gint b, gint x0, gint y0, gint width, gint height)
{
	GList *list;
	gint x, y;
	gint e1, e2, e3;
	gboolean in_zone;

	list = NULL;

	x = a;
	y = 0;

	add_pixel_in_zone(&list, x, y, x0, y0, width, height);
	add_pixel_in_zone(&list, -x, y, x0, y0, width, height);

	while (TRUE) {
		e1 = abs(SQR(x + 1) * SQR(b) - SQR(y + 1) * SQR(a) - SQR(a) * SQR(b));
		e2 = abs(SQR(x + 1) * SQR(b) - SQR(y) * SQR(a) - SQR(a) * SQR(b));
		e3 = abs(SQR(x) * SQR(b) - SQR(y + 1) * SQR(a) - SQR(a) * SQR(b));

		if (e1 < e2 && e1 < e3) {
			++x;
			++y;
		} else if (e2 < e3) {
			++x;
		} else {
			++y;
		}

		in_zone = FALSE;
		in_zone |= add_pixel_in_zone(&list, x, y, x0, y0, width, height);
		in_zone |= add_pixel_in_zone(&list, x, -y, x0, y0, width, height);
		in_zone |= add_pixel_in_zone(&list, -x, y, x0, y0, width, height);
		in_zone |= add_pixel_in_zone(&list, -x, -y, x0, y0, width, height);
		if (!in_zone) break;
	}

	return list;
}

static mat4 b_spline = {
		{-1, 3, -3, 1},
		{3, -6, 0, 4},
		{-3, 3, 3, 1},
		{1, 0, 0, 0}
};

GList *
get_b_spline_figure(GList *points, gdouble step)
{
	GList *figure;
    Point *point;
    gdouble t;
    gint i;
    gint x, y;
    gdouble double_x, double_y;
    vec4 result;
    gint n;

    n = g_list_length(points);
	figure  = NULL;

    gdouble array[2][n + 4];

    point = points->data;
    array[0][0] = array[0][1] = array[0][2] = point->x;
    array[1][0] = array[1][1] = array[1][2] = point->y;
    for (i = 3; i < n + 2; ++i) {
        points = g_list_next(points);
        point = points->data;

        array[0][i] = point->x;
        array[1][i] = point->y;
    }
    array[0][i] = array[0][i + 1] = point->x;
    array[1][i] = array[1][i + 1] = point->y;


	for (i = 1; i <= n + 1; ++i) {
		for (t = 0; t < 1 + 1e-5; t += step) {
			vec4 vec_t = {pow(t, 3), pow(t, 2), t, 1};

            multiplication_mat4_vec4(result, b_spline, vec_t);
            multiplication_vec4_vec4(&double_x, array[0] + i - 1, result);
            multiplication_vec4_vec4(&double_y, array[1] + i - 1, result);

			x = round(double_x / 6);
			y = round(double_y / 6);

			add_pixel(&figure, x, y);
		}
	}

	return figure;
}
