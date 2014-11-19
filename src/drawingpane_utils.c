#include "drawingpane_utils.h"
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
	gint dx, dy;
	gdouble e;
	gdouble alpha, temp;
	gboolean is_swaped = FALSE;

	figure = NULL;
	dx = x2 - x1;
	dy = y2 - y1;

	if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
		return get_bresenham_line_figure(x1, y1, x2, y2);
	}

	if (abs(dx) > abs(dy)) {
		if (x1 > x2) {
			is_swaped = TRUE;
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		e = (gdouble) dy / dx;

		gint x;
		gdouble y;

		add_pixel(&figure, x1, y1);
		for (x = x1, y = y1 - 0.5 * sign(e); x <= x2; ++x) {
			alpha = modf(y, &temp);

			if (sign(e) < 0) alpha = 1 - alpha;

			add_pixel_with_alpha(&figure, x, floor(y), 1 - alpha);
			add_pixel_with_alpha(&figure, x, floor(y) + sign(e), alpha);

			y += e;
		}
		add_pixel(&figure, x2, y2);
	} else {
		if (y1 > y2) {
			is_swaped = TRUE;
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		e = (gdouble)dx / dy;

		gint y;
		gdouble x;

		add_pixel(&figure, x1, y1);

		for (x = x1 - 0.5 * sign(e), y = y1; y <= y2; ++y) {
			alpha = modf(x, &temp);

			if (sign(e) < 0) alpha = 1 - alpha;

			add_pixel_with_alpha(&figure, floor(x), y, 1 - alpha);
			add_pixel_with_alpha(&figure, floor(x) + sign(e), y, alpha);

			x += e;
		}

		add_pixel(&figure, x2, y2);
	}

	if (is_swaped == TRUE) {
		figure = g_list_reverse(figure);
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
