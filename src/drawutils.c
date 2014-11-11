#include "drawutils.h"

#include <math.h>
#include <gtk/gtk.h>

static gdouble height = 20;
static gdouble width = 20;
static gdouble cell_size = 1;

static void draw_net(cairo_t* cr) {
	if (cell_size < 10) return;

	cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

	gdouble i;
	gdouble h = height * 2 + 1;
	gdouble w = width * 2 + 1;
	gdouble real_height = h * cell_size;
	gdouble real_weigth = w * cell_size;

	for (i = 0; i - 1e-5 <= h; ++i) {
		cairo_move_to(cr, i * cell_size, 0);
		cairo_line_to(cr, i * cell_size, real_height);
	}

	for (i = 0; i - 1e-5 <= w; ++i) {
		cairo_move_to(cr, 0, i * cell_size);
		cairo_line_to(cr, real_weigth, i * cell_size);
	}

	cairo_stroke(cr);
}

static void draw_pixel(cairo_t* cr, gint x, gint y) {
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, (x + width) * cell_size, (height - y) * cell_size, cell_size, cell_size);
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

	gdouble x = x1 + 0.5 * sign(dx);
	gdouble y = y1 + 0.5 * sign(dy);
	draw_pixel(cr, floor(x), floor(y));

	gint i = 0;
	while (i <= length) {
		x += dx;
		y += dy;
		draw_pixel(cr, floor(x), floor(y));
		++i;
	}

}

static void draw_line2(cairo_t* cr, gint x1, gint y1, gint x2, gint y2) {
	gint x = x1;
	gint y = y1;
	gint dx = x2 - x1;
	gint dy = y2 - y1;
	draw_pixel(cr, x, y);
	gint i = 1;
	gint e;

	gint inc_x = dx > 0 ? 1 : -1;
	gint inc_y = dy > 0 ? 1 : -1;

	dx = abs(dx);
	dy = abs(dy);

	if (dx > dy) {
		e = 2 * dy - dx;
		while (i <= dx) {
			if (e >= 0) {
				y += inc_y;
				e -= 2 * dx;
			}
			x += inc_x;
			e += 2 * dy;
			++i;
			draw_pixel(cr, x, y);
		}
	} else {
		e = 2 * dx - dy;
		while (i <= dy) {
			if (e >= 0) {
				x += inc_x;
				e -= 2 * dy;
			}
			y += inc_y;
			e += 2 * dx;
			++i;
			draw_pixel(cr, x, y);
		}
	}
}

gboolean draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	draw_line1(cr, -3, 1, 5, 4);
	draw_line2(cr, 5, -1, 4, 5);
	draw_net(cr);

	return FALSE;
}

gboolean configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	cell_size = gtk_widget_get_allocated_height(widget) / (height * 2 + 1);

	return TRUE;
}

