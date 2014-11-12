#include "drawutils.h"

#include <math.h>
#include <gtk/gtk.h>

static gint height = 20;
static gint width = 20;
static gint cell_size = 1;

static void draw_net(cairo_t* cr) {
	gint net_size = cell_size / 10;
	if (net_size == 0) return;

	cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

	gdouble i;
	gdouble real_height = height * cell_size;
	gdouble real_width = width * cell_size;

	for (i = 0; i <= width; ++i) {
		cairo_rectangle(cr, i * cell_size - net_size / 2, 0, net_size, real_height);
	}

	for (i = 0; i <= height; ++i) {
		cairo_rectangle(cr, 0, i * cell_size - net_size / 2, real_width, net_size);
	}

	cairo_fill(cr);
}

static void draw_pixel(cairo_t* cr, gint x, gint y) {
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, x * cell_size, y * cell_size, cell_size, cell_size);
	cairo_fill(cr);
}

static gint sign(gdouble x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

static void draw_line1(cairo_t* cr, gint x1, gint y1, gint x2, gint y2) {
	gint length = MAX(abs(x2 - x1), abs(y2 - y1));

	gdouble dx = (x2 - x1) / (gfloat)length;
	gdouble dy = (y2 - y1) / (gfloat)length;

	gdouble x = x1;
	gdouble y = y1;

	gint i;
	for (i = 0; i <= length; ++i) {
		draw_pixel(cr, round(x), round(y));
		x += dx;
		y += dy;
	}

}

static void swap(gint *a, gint *b) {
	gint o = *a;
	*a = *b;
	*b = o;
}

static void draw_line2(cairo_t* cr, gint x1, gint y1, gint x2, gint y2) {
	gint dx = abs(x2 - x1);
	gint dy = abs(y2 - y1);

	gint e, x, y;

	if (dx > dy) {
		if (x1 > x2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		gint inc_y = sign(y2 - y1);
		e = dy;

		for (x = x1, y = y1; x <= x2; ++x) {
			draw_pixel(cr, x, y);

			if (2 * e >= dx) {
				y += inc_y;
				e -= dx;
			}

			e += dy;
		}
	} else {
		if (y1 > y2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		gint inc_x = sign(x2 - x1);
		e = dx;

		for (x = x1, y = y1; y <= y2; ++y) {
			draw_pixel(cr, x, y);

			if (2 * e >= dy) {
				x += inc_x;
				e -= dy;
			}

			e += dx;
		}
	}

}

gboolean draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	draw_line1(cr, 0, 0, 5, 2);
	draw_net(cr);

	return FALSE;
}

gboolean configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	cell_size = gtk_widget_get_allocated_height(widget) / height;

	return TRUE;
}

