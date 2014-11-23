#include "drawingpane.h"
#include "drawingpane_utils.h"
#include "graphicseditor_utils.h"

#include <math.h>

#define STEP 0.005

typedef struct _Color Color;
struct _Color {
    gdouble r, g, b;
};

typedef struct _Spline Spline;
struct _Spline
{
    GList *points;
    GList *pixels;
    gboolean need_refresh_pixels;
};

struct _DrawingPanePrivate
{
	GraphicsEditorWindow *window;

	gint cell_size;
	gint width, height;
	GList *figure_list;
	GList *editing_line;

	cairo_surface_t *surface;
	GtkDrawingArea *drawing_area;
	GtkScrolledWindow *scrolled_window;

	gdouble width_before_scale;
	gdouble height_before_scale;
	gdouble scale_x_fixed;
	gdouble scale_y_fixed;
	gboolean was_scaled;

	GList *b_spline_points;
	GList *b_spline_pixels;
    GList *b_spliens;
    Point *old_point;
    Spline *move_spline;
};

static Color red_color = {1, 0, 0};
static Color green_color = {0, 1, 0};
static Color blue_color = {0, 0, 1};

static void drawing_pane_constructed(GObject *obj);
static void drawing_pane_finalize(GObject *obj);
static void drawing_pane_set_handlers(DrawingPane *pane);
static gboolean drawing_area_scroll_event_handler(GtkWidget *drawing_area, GdkEventScroll *event, gpointer user_data);
static gboolean drawing_area_draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean drawing_area_configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean drawing_area_button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data);
static gboolean drawing_area_motion_notify_event_handler (GtkWidget *widget, GdkEventMotion  *event, gpointer data);
static gboolean drawing_area_button_realese_event_handler (GtkWidget *widget, GdkEventButton *event, gpointer data);
static void draw_pixel(cairo_t *cr, Pixel *pixel, DrawingPane *pane);
static void draw_figure(cairo_t *cr, GList *figure, DrawingPane *pane);
static void translate(DrawingPane *pane, gint *x, gint *y);
static void clear_list(GList **figure);
static void init_surface(DrawingPane *pane);
static void refresh_surface(DrawingPane *pane);
static GraphicsEditorDrawingModeType get_drawing_mode(DrawingPane *pane);
static gboolean is_line_drawing_mode(GraphicsEditorDrawingModeType mode);
static void move_line_end(GraphicsEditorDrawingModeType drawing_mode, GList **figure, gint x2, gint y2);
static GList *get_line_figure(GraphicsEditorDrawingModeType drawing_mode, gint x1, gint y1, gint x2, gint y2);
static GList *get_hyperbole(DrawingPane *pane);
static void draw_net(cairo_t* cr, DrawingPane *pane);
static void draw_point(cairo_t *cr, Point *point, Color color);
static void draw_key_points(cairo_t *cr, GList *list, Color color);
static void get_nearest_point_to(gint x, gint y, DrawingPane *pane, Spline **out_spline, Point **out_point);

G_DEFINE_TYPE_WITH_PRIVATE(DrawingPane, drawing_pane, GTK_TYPE_BIN)

static gboolean
drawing_area_scroll_event_handler(GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
	DrawingPanePrivate *priv;

	priv = DRAWING_PANE(user_data)->priv;

	if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK) {
		gint direction;

		switch (event->direction) {
		case GDK_SCROLL_UP:
			direction = 1;
			break;
		case GDK_SCROLL_DOWN:
			direction = -1;
			break;
		default:
			return TRUE;
		}

		gint widget_width, new_width;
		gint widget_height, new_height;

		gtk_widget_get_size_request(GTK_WIDGET(priv->drawing_area), &widget_width, &widget_height);

		new_width = widget_width + direction * priv->width;
		new_height = widget_height + direction * priv->height;

		if (new_width > 0 &&
				new_height > 0 &&
				new_width <= priv->width * 100 &&
				new_height <= priv->height * 100) {

			if (!priv->was_scaled) {
				priv->scale_x_fixed = event->x;
				priv->scale_y_fixed = event->y;
				priv->width_before_scale = widget_width;
				priv->height_before_scale = widget_height;
				priv->was_scaled = TRUE;
			}

			gtk_widget_set_size_request(GTK_WIDGET(priv->drawing_area), new_width, new_height);
		}

		return TRUE;
	}

	return FALSE;
}

static void
drawing_pane_init (DrawingPane *pane)
{
	gtk_widget_init_template(GTK_WIDGET(pane));
	pane->priv = drawing_pane_get_instance_private(pane);
	pane->priv->cell_size = 1;
	pane->priv->height = DRAWING_PANE_DEFAULT_HEIGHT;
	pane->priv->width = DRAWING_PANE_DEFAULT_WIDTH;
	pane->priv->was_scaled = FALSE;
	pane->priv->b_spline_pixels = NULL;
	pane->priv->b_spline_points = NULL;
    pane->priv->b_spliens = NULL;
    pane->priv->move_spline = NULL;
    pane->priv->old_point = NULL;
}

static void
drawing_pane_class_init (DrawingPaneClass *class)
{
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(class);

	object_class->constructed = drawing_pane_constructed;
	object_class->finalize = drawing_pane_finalize;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
			"/by/jylilov/graphicseditor/drawing_pane.xml");

	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DrawingPane, drawing_area);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DrawingPane, scrolled_window);
}

static void
drawing_pane_constructed(GObject *obj)
{
	DrawingPanePrivate *priv;

	priv = DRAWING_PANE(obj)->priv;

	if (G_OBJECT_CLASS (drawing_pane_parent_class)->constructed != NULL)
		G_OBJECT_CLASS (drawing_pane_parent_class)->constructed (obj);

	gtk_widget_set_size_request(GTK_WIDGET(priv->drawing_area), priv->width, priv->height);

	drawing_pane_set_handlers(DRAWING_PANE(obj));
}

static void
drawing_pane_set_handlers(DrawingPane *pane)
{
	DrawingPanePrivate *priv;
	priv = pane->priv;

	gtk_widget_set_events (GTK_WIDGET(priv->drawing_area), gtk_widget_get_events (GTK_WIDGET(priv->drawing_area))
			| GDK_SCROLL_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_BUTTON_PRESS_MASK
            | GDK_BUTTON_RELEASE_MASK);

	g_signal_connect(priv->drawing_area,
			"scroll-event",
			G_CALLBACK(drawing_area_scroll_event_handler),
			pane);

	g_signal_connect(priv->drawing_area,
			"draw",
			G_CALLBACK(drawing_area_draw_handler),
			pane);

	g_signal_connect(priv->drawing_area,
			"configure-event",
			G_CALLBACK(drawing_area_configure_event_handler),
			pane);

	g_signal_connect(priv->drawing_area,
			"motion-notify-event",
			G_CALLBACK(drawing_area_motion_notify_event_handler),
			pane);

	g_signal_connect(priv->drawing_area,
			"button-press-event",
			G_CALLBACK(drawing_area_button_press_event_handler),
			pane);

    g_signal_connect(priv->drawing_area,
            "button-release-event",
            G_CALLBACK(drawing_area_button_realese_event_handler),
            pane);

}

static void
drawing_pane_finalize(GObject *obj)
{
	DrawingPanePrivate *priv;

	//TODO

	if (G_OBJECT_CLASS (drawing_pane_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (drawing_pane_parent_class)->finalize (obj);
}

DrawingPane *
drawing_pane_new (GraphicsEditorWindow *win)
{
	return drawing_pane_new_with_size(win, DRAWING_PANE_DEFAULT_WIDTH, DRAWING_PANE_DEFAULT_HEIGHT);
}

DrawingPane *
drawing_pane_new_with_size (GraphicsEditorWindow *win, gint width, gint height)
{
	DrawingPane *pane = g_object_new(DRAWING_PANE_TYPE, NULL);

	pane->priv->window = win;
	pane->priv->width = width;
	pane->priv->height = height;

	return pane;
}

static void
draw_pixel(cairo_t *cr, Pixel *pixel, DrawingPane *pane)
{
	cairo_set_source_rgba(cr, 0, 0, 0, pixel->alpha);
	cairo_rectangle(cr,
			//pixel->x + pane->priv->width / 2, pixel->y + pane->priv->height / 2,
			pixel->x, pixel->y,
			1, 1);
	cairo_fill(cr);
}

static void
draw_figure(cairo_t *cr, GList *figure, DrawingPane *pane) {
	Pixel *pixel;

	while (figure != NULL) {
		pixel = figure->data;
		draw_pixel(cr, pixel, pane);
		figure = g_list_next(figure);
	}
}

static GraphicsEditorDrawingModeType
get_drawing_mode(DrawingPane *pane) {
	GraphicsEditorDrawingModeType answer;
	g_object_get(G_OBJECT(pane->priv->window), "drawing-mode", &answer, NULL);
	return answer;
}

static gboolean
is_line_drawing_mode(GraphicsEditorDrawingModeType mode) {
	return (mode == GRAPHICSEDITOR_DRAWING_MODE_DDA_LINE ||
			mode == GRAPHICSEDITOR_DRAWING_MODE_BRESENHAM_LINE ||
			mode == GRAPHICSEDITOR_DRAWING_MODE_WU_LINE);
}

static void
init_surface(DrawingPane *pane) {
	if (pane->priv->surface == NULL) {
		pane->priv->surface = gdk_window_create_similar_surface (
				gtk_widget_get_window (GTK_WIDGET(pane->priv->drawing_area)),
				CAIRO_CONTENT_COLOR,
				pane->priv->width,
				pane->priv->height);
		refresh_surface(pane);
	}
}

static void
refresh_surface(DrawingPane *pane) {
	DrawingPanePrivate *priv;
	cairo_t *cr;
	GList *figure, *figure_list;
    Spline *spline;

	priv = DRAWING_PANE(pane)->priv;
	cr = cairo_create (priv->surface);

	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_paint (cr);

    // Adding on surface lines(1st and 2nd order)

    figure_list = priv->figure_list;
	while (figure_list != NULL) {
		figure = figure_list->data;
		draw_figure(cr, figure, pane);
		figure_list = g_list_next(figure_list);
	}

    // Adding on surface splines

    figure_list = priv->b_spliens;
    while (figure_list != NULL) {
        spline = figure_list->data;
        if (spline->need_refresh_pixels) {
            spline->pixels = get_b_spline_figure(spline->points, STEP);
            spline->need_refresh_pixels = FALSE;
        }
        draw_figure(cr, spline->pixels, pane);
        figure_list = g_list_next(figure_list);
    }

	cairo_destroy (cr);
}

gboolean
drawing_area_draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data) {
    DrawingPanePrivate *priv;
    GraphicsEditorDrawingModeType drawing_mode;
    Spline *spline;
    GList *list;

    priv = DRAWING_PANE(data)->priv;
    drawing_mode = get_drawing_mode(DRAWING_PANE(data));

    cairo_scale(cr, priv->cell_size, priv->cell_size);

    //Drawing static surface

    cairo_set_source_surface(cr, priv->surface, 0, 0);
    cairo_paint(cr);

    //Drawing figures which are creating

    if (priv->editing_line != NULL) {
        draw_figure(cr, priv->editing_line, DRAWING_PANE(data));
    }

    if (priv->b_spline_pixels)
        draw_figure(cr, priv->b_spline_pixels, DRAWING_PANE(data));

    //Drawing key points

    if (drawing_mode == GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE) {
        draw_key_points(cr, priv->b_spline_points, green_color);

        list = priv->b_spliens;
        while (list != NULL) {
            spline = list->data;
            draw_key_points(cr, spline->points, blue_color);
            list = g_list_next(list);
        }
    }

    if (priv->old_point != NULL) {
        draw_point(cr, priv->old_point, red_color);
    }

    //Finish drawing figures
	cairo_scale(cr, 1.0 / priv->cell_size, 1.0 / priv->cell_size);

    //Drawing net;
	draw_net(cr, DRAWING_PANE(data));

	return TRUE;
}

static void
draw_key_points(cairo_t *cr, GList *list, Color color)
{
    gint i, n;
    Point *point;
    Color boundary_color;

    i = 0;

    boundary_color = color;
    boundary_color.r *= 0.5;
    boundary_color.b *= 0.5;
    boundary_color.g *= 0.5;

    n = g_list_length(list);
    while (list != NULL) {
        i++;

		point = list->data;
        if (i == 1 || i == n) {
            draw_point(cr, point, boundary_color);
        } else {
            draw_point(cr, point, color);
        }

        list = g_list_next(list);
	}
}

static void
draw_point(cairo_t *cr, Point *point, Color color)
{
    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb(cr, color.r, color.g, color.b);
    cairo_arc(cr, point->x, point->y, 5, 0, 2 * M_PI);
    cairo_fill_preserve(cr);

    cairo_set_source_rgb(cr, color.r * 0.5, color.g * 0.5, color.b * 0.5);
    cairo_stroke(cr);
}

gboolean
drawing_area_configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	DrawingPanePrivate *priv;

	priv = DRAWING_PANE(data)->priv;

	gint current_width;
	gint current_height;

	gtk_widget_get_size_request(GTK_WIDGET(priv->drawing_area), &current_width, &current_height);

	if (priv->was_scaled) {
		GtkAdjustment *hadjustment = gtk_scrolled_window_get_hadjustment(priv->scrolled_window);
		GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(priv->scrolled_window);

		gdouble vadj_value = gtk_adjustment_get_value(vadjustment);
		gdouble hadj_value = gtk_adjustment_get_value(hadjustment);

		// if drawing_area does not occupy the entire scrolled_window
		if (vadj_value == 0) {
			vadj_value = (priv->height_before_scale -
					gtk_widget_get_allocated_height(GTK_WIDGET(priv->scrolled_window))) / 2;
		}

		if (hadj_value == 0) {
			hadj_value = (priv->width_before_scale -
					gtk_widget_get_allocated_width(GTK_WIDGET(priv->scrolled_window))) / 2;
		}

		gdouble goal_hadjustment_value = priv->scale_x_fixed * current_width / priv->width_before_scale
				- priv->scale_x_fixed + hadj_value;
		gdouble goal_vadjustment_value = priv->scale_y_fixed * current_height / priv->height_before_scale
				- priv->scale_y_fixed + vadj_value;

		gtk_adjustment_set_value(hadjustment, goal_hadjustment_value);
		gtk_adjustment_set_value(vadjustment, goal_vadjustment_value);

		priv->was_scaled = FALSE;
	}

	priv->cell_size = current_width / priv->width;

	init_surface(DRAWING_PANE(data));

	gtk_widget_queue_draw(GTK_WIDGET(priv->drawing_area));

	return TRUE;
}

static gboolean
is_point_boundary(Point *point, Spline *spline) {
    return g_list_first(spline->points)->data == point
            || g_list_last(spline->points)->data == point;
}

static gboolean
drawing_area_button_realese_event_handler (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    DrawingPanePrivate *priv;
    GraphicsEditorDrawingModeType drawing_mode;
    gint x, y;

    priv = DRAWING_PANE(data)->priv;
    drawing_mode = get_drawing_mode(DRAWING_PANE(data));

    if (drawing_mode == GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE) {
        if (priv->move_spline != NULL) {
            x = floor(event->x / priv->cell_size);
            y = floor(event->y / priv->cell_size);

            translate(DRAWING_PANE(data), &x, &y);

            Point *near_point = NULL;
            Spline *near_spline = NULL;

            get_nearest_point_to(x, y, DRAWING_PANE(data), &near_spline, &near_point);


            if (near_point != NULL && priv->old_point != near_point && near_spline != priv->move_spline
                    && is_point_boundary(priv->old_point, priv->move_spline)
                    && is_point_boundary(near_point, near_spline)) {
                if (priv->old_point != priv->move_spline->points->data) {
                    priv->move_spline->points = g_list_reverse(priv->move_spline->points);
                }
                if (near_spline->points->data == near_point) {
                    near_spline->points = g_list_reverse(near_spline->points);
                }

                GList *list;
                list = priv->move_spline->points;

                while (list != NULL) {
                    near_spline->points = g_list_append(near_spline->points, list->data);
                    list = g_list_next(list);
                }

                clear_list(&priv->move_spline->pixels);
                g_list_free(priv->move_spline->points);
                priv->b_spliens = g_list_remove(priv->b_spliens, priv->move_spline);
                g_free(priv->move_spline);

                near_spline->need_refresh_pixels = TRUE;

                priv->old_point = NULL;
                priv->move_spline = NULL;
            } else {
                priv->old_point->x = x;
                priv->old_point->y = y;

                priv->move_spline->need_refresh_pixels = TRUE;

                priv->move_spline = NULL;
                priv->old_point = NULL;
            }
        }

        refresh_surface(DRAWING_PANE(data));
        gtk_widget_queue_draw(GTK_WIDGET(priv->drawing_area));

    }

    return FALSE;
}

static void
get_nearest_point_to(gint x, gint y, DrawingPane *pane, Spline **out_spline, Point **out_point)
{
    GList *spline_list, *point_list;
    DrawingPanePrivate *priv;
    Spline *spline;
    Point *point;

    priv = pane->priv;

    spline_list = priv->b_spliens;
    while (spline_list != NULL) {
        spline = spline_list->data;
        point_list = spline->points;
        while (point_list != NULL) {
            point = point_list->data;
            if (hypot(point->x - x, point->y - y) < 15) {
                *out_spline = spline;
                *out_point = point;
                return;
            }
            point_list = g_list_next(point_list);
        }
        spline_list = g_list_next(spline_list);
    }
}

// TODO then switch "drawing-mode" reset editing_line, etc.
static gboolean
drawing_area_button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data)
{
	DrawingPanePrivate *priv;
    Point *point;
    Spline *spline;
    GList *spline_list, *point_list;
	gint x, y;
	GraphicsEditorDrawingModeType drawing_mode;
    drawing_mode = get_drawing_mode(DRAWING_PANE(data));

	priv = DRAWING_PANE(data)->priv;
    if (is_line_drawing_mode(drawing_mode)) {
        switch(event->button) {
        case 1:
            x = floor(event->x / priv->cell_size);
            y = floor(event->y / priv->cell_size);
            translate(DRAWING_PANE(data), &x, &y);
            if (priv->editing_line == NULL) {
                priv->editing_line = get_line_figure(drawing_mode, x, y, x, y);
            } else {
                move_line_end(drawing_mode, &priv->editing_line, x, y);

                priv->figure_list = g_list_append(priv->figure_list, priv->editing_line);
                priv->editing_line = NULL;

                refresh_surface(DRAWING_PANE(data));
            }
            break;
        case 3:
            clear_list(&priv->editing_line);
            break;
        }

    } else if (drawing_mode == GRAPHICSEDITOR_DRAWING_MODE_HYPERBOLE) {
        GList *hyperbole = get_hyperbole(DRAWING_PANE(data));
        if (hyperbole) {
            priv->figure_list = g_list_append(priv->figure_list, hyperbole);
            refresh_surface(DRAWING_PANE(data));
        }
    } else if (drawing_mode == GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE) {
        switch (event->button) {
        case 1:
            x = floor(event->x / priv->cell_size);
            y = floor(event->y / priv->cell_size);

            if (priv->b_spline_points == NULL) {
                get_nearest_point_to(x, y, DRAWING_PANE(data), &priv->move_spline, &priv->old_point);
            }

            if (priv->move_spline == NULL) {
                point = g_malloc(sizeof(Point));
                point->x = x;
                point->y = y;

                priv->b_spline_points = g_list_append(priv->b_spline_points, point);

                //translate(DRAWING_PANE(data), &x, &y);

                clear_list(&priv->b_spline_pixels);
                priv->b_spline_pixels = get_b_spline_figure(priv->b_spline_points, STEP);
            }

            break;
        case 3:
            if (priv->b_spline_points != NULL) {
                spline = g_malloc(sizeof(Spline));

                spline->need_refresh_pixels = FALSE;
                spline->pixels = priv->b_spline_pixels;
                spline->points = priv->b_spline_points;

                priv->b_spliens = g_list_append(priv->b_spliens, spline);
                priv->b_spline_pixels = NULL;
                priv->b_spline_points = NULL;

                refresh_surface(DRAWING_PANE(data));
            }
            break;
        }

    }

	gtk_widget_queue_draw(widget);

	return FALSE;
}

static void clear_list(GList **figure) {
	gpointer obj;
	while (*figure != NULL) {
		obj = (*figure)->data;
		*figure = g_list_remove(*figure, obj);
		g_free(obj);
	}
}

static GList *
get_line_figure(GraphicsEditorDrawingModeType drawing_mode, gint x1, gint y1, gint x2, gint y2) {
	GList *figure;

	figure = NULL;
	switch (drawing_mode) {
	case GRAPHICSEDITOR_DRAWING_MODE_DDA_LINE:
		figure = get_dda_line_figure(
				x1, y1,
				x2, y2);
		break;
	case GRAPHICSEDITOR_DRAWING_MODE_BRESENHAM_LINE:
		figure = get_bresenham_line_figure(
				x1, y1,
				x2, y2);
		break;
	case GRAPHICSEDITOR_DRAWING_MODE_WU_LINE:
		figure = get_wu_line_figure(
				x1, y1,
				x2, y2);
		break;
	}

	return figure;
}

static void
move_line_end(GraphicsEditorDrawingModeType drawing_mode, GList **figure, gint x2, gint y2)
{
	Pixel *start_pixel;
	gint x1, y1;

	start_pixel = (*figure)->data;

	x1 = start_pixel->x;
	y1 = start_pixel->y;

    clear_list(figure);

	*figure = get_line_figure(drawing_mode, x1, y1, x2, y2);
}


//TODO Drawing net optimization
static void draw_net(cairo_t* cr, DrawingPane *pane) {
	gint cell_size = pane->priv->cell_size;
	gint height = pane->priv->height;
	gint width = pane->priv->width;
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

gboolean
drawing_area_motion_notify_event_handler (GtkWidget *widget, GdkEventMotion  *event, gpointer data)
{
	DrawingPanePrivate *priv;
	gint x, y;
	GraphicsEditorDrawingModeType drawing_mode;

	priv = DRAWING_PANE(data)->priv;
    drawing_mode = get_drawing_mode(DRAWING_PANE(data));

    if (is_line_drawing_mode(drawing_mode)) {
        if (priv->editing_line != NULL) {
            x = floor(event->x / priv->cell_size);
            y = floor(event->y / priv->cell_size);

            translate(DRAWING_PANE(data), &x, &y);

            move_line_end(drawing_mode, &priv->editing_line, x, y);

            gtk_widget_queue_draw(widget);
        }
    }

	return FALSE;
}

static GList *
get_hyperbole(DrawingPane *pane)
{
	GList *figure;
	GtkWidget *dialog;
	GtkWidget *grid;
	GtkWidget *content_area;
	GtkWidget *spin_button_a;
	GtkWidget *spin_button_b;
	GtkWidget *label_a;
	GtkWidget *label_b;
	gint a;
	gint b;

	figure = NULL;

	dialog = gtk_dialog_new_with_buttons("Add hyperbole",
			GTK_WINDOW(pane->priv->window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			"OK", GTK_RESPONSE_OK,
			"Cancel", GTK_RESPONSE_CANCEL,
			NULL);

	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	grid = gtk_grid_new();

	label_a = gtk_label_new("Hyperbole parameter \"a\" :");
	label_b = gtk_label_new("Hyperbole parameter \"b\" :");
	spin_button_a = gtk_spin_button_new_with_range(1, 10000, 100);
	spin_button_b = gtk_spin_button_new_with_range(1, 10000, 100);

	gtk_grid_attach(GTK_GRID(grid), label_a, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), spin_button_a, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), label_b, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), spin_button_b, 1, 1, 1, 1);

	gtk_container_add(GTK_CONTAINER(content_area), grid);
	gtk_widget_show_all(GTK_WIDGET(grid));

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	a = round(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_a)));
	b = round(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_b)));
	gtk_widget_destroy(dialog);

	if (result == GTK_RESPONSE_OK) {
		figure = get_hyperbole_figure(a, b,
				0, 0,
				pane->priv->width, pane->priv->height);
	}

	return figure;
}

//TODO
static void
translate(DrawingPane *pane, gint *x, gint *y) {
	//*x -= pane->priv->width / 2;
	//*y -= pane->priv->height / 2;
	//*y = -1 * *y;
}
