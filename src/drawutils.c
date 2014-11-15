#include "drawutils.h"

#include <math.h>

typedef struct _Point Point;
typedef struct _Line Line;

struct _Point {
	gint x, y;
};

struct _Line {
	Point p1, p2;
};

static gint height;
static gint width;
static gint cell_size = 1;

static gint drawing_mode = 0;

static GSList *dda_lines;
static GSList *bresenham_lines;
static GSList *wu_lines;
static Line *current_line = NULL;

static void draw_net(cairo_t* cr) {
	gint net_size = cell_size / 7;
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

static void draw_pixel_with_alpha(cairo_t* cr, gint x, gint y, gdouble alpha) {
	cairo_set_source_rgb(cr, 1 - alpha, 1 - alpha, 1 - alpha);
	cairo_rectangle(cr, x * cell_size, y * cell_size, cell_size, cell_size);
	cairo_fill(cr);
}

static void draw_pixel(cairo_t* cr, gint x, gint y) {
	draw_pixel_with_alpha(cr, x, y, 1);
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

static void draw_line3(cairo_t* cr, gint x1, gint y1, gint x2, gint y2) {
	gint dx = x2 - x1;
	gint dy = y2 - y1;

	if (dx == 0 || dy == 0) {
		draw_line2(cr, x1, y1, x2, y2);
		return;
	}

	gdouble e;

	gdouble alpha, temp;

	if (abs(dx) > abs(dy)) {
		if (x1 > x2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		e = (gdouble) dy / dx;

		gint x;
		gdouble y;

		for (x = x1, y = y1; x <= x2; ++x) {
			alpha = modf(y, &temp);

			if (sign(e) < 0) alpha = 1 - alpha;

			draw_pixel_with_alpha(cr, x, floor(y), 1 - alpha);
			draw_pixel_with_alpha(cr, x, floor(y) + sign(e), alpha);

			y += e;
		}
	} else {
		if (y1 > y2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		e = (gdouble)dx / dy;

		gint y;
		gdouble x;

		for (x = x1, y = y1; y <= y2; ++y) {
			alpha = modf(x, &temp);

			if (sign(e) < 0) alpha = 1 - alpha;

			draw_pixel_with_alpha(cr, floor(x), y, 1 - alpha);
			draw_pixel_with_alpha(cr, floor(x) + sign(e), y, alpha);

			x += e;
		}
	}

}

gboolean draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	GSList *list;
	Line *line;

	list = dda_lines;
	while (list != NULL) {
		line = list->data;
		draw_line1(cr, line->p1.x, line->p1.y, line->p2.x, line->p2.y);
		list = g_slist_next(list);
	}

	list = bresenham_lines;
	while (list != NULL) {
		line = list->data;
		draw_line2(cr, line->p1.x, line->p1.y, line->p2.x, line->p2.y);
		list = g_slist_next(list);
	}

	list = wu_lines;
	while (list != NULL) {
		line = list->data;
		draw_line3(cr, line->p1.x, line->p1.y, line->p2.x, line->p2.y);
		list = g_slist_next(list);
	}

	if (current_line != NULL) {
		switch(drawing_mode) {
		case DRAWING_MODE_LINE_DDA:
			draw_line1(cr, current_line->p1.x, current_line->p1.y, current_line->p2.x, current_line->p2.y);
			break;
		case DRAWING_MODE_LINE_BRESENHAM:
			draw_line2(cr, current_line->p1.x, current_line->p1.y, current_line->p2.x, current_line->p2.y);
			break;
		case DRAWING_MODE_LINE_WU:
			draw_line3(cr, current_line->p1.x, current_line->p1.y, current_line->p2.x, current_line->p2.y);
			break;
		}
	}

	draw_net(cr);
	return FALSE;
}

gboolean configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	cell_size = gtk_widget_get_allocated_height(widget) / height;
	gtk_widget_queue_draw(widget);
	return TRUE;
}

static void clear_current_line(void)
{
	if (current_line != NULL) {
		free(current_line);
		current_line = NULL;
	}
}

static void add_line(Line *line) {
	switch (drawing_mode) {
	case DRAWING_MODE_LINE_DDA:
		dda_lines = g_slist_append(dda_lines, line);
		break;
	case DRAWING_MODE_LINE_BRESENHAM:
		bresenham_lines = g_slist_append(bresenham_lines, line);
		break;
	case DRAWING_MODE_LINE_WU:
		wu_lines = g_slist_append(wu_lines, line);
		break;
	}
}

gboolean button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data)
{
	switch (drawing_mode) {
	case DRAWING_MODE_LINE_DDA:
	case DRAWING_MODE_LINE_BRESENHAM:
	case DRAWING_MODE_LINE_WU:
		if (current_line == NULL) {
			current_line = malloc(sizeof(Line));
			current_line->p1.x = current_line->p2.x = floor(event->x / cell_size);
			current_line->p1.y = current_line->p2.y = floor(event->y / cell_size);
		} else {
			add_line(current_line);
			current_line = NULL;
			gtk_widget_queue_draw(widget);
		}
		break;
	case DRAWING_MODE_NONE:
		clear_current_line();
		break;
	}
	return TRUE;
}
gboolean motion_notify_event_handler (GtkWidget *widget, GdkEventMotion  *event, gpointer data)
{
	if (current_line != NULL) {
		current_line->p2.x = event->x / cell_size;
		current_line->p2.y = event->y / cell_size;
		gtk_widget_queue_draw(widget);
	}
	return TRUE;
}

void drawutils_set_drawing_mode(gint mode) {
	drawing_mode = mode;
	clear_current_line();
}

gint drawutils_get_width() {
	return width;
}

gint drawutils_get_height() {
	return height;
}

gint drawutils_get_cell_size() {
	return cell_size;
}

void drawutils_init(GtkDrawingArea *drawing_area, gint area_width, gint area_height) {
	width = area_width;
	height = area_height;
	gtk_widget_set_size_request(GTK_WIDGET(drawing_area), area_width, area_height);
}

