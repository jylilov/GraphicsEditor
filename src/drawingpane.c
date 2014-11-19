#include "drawingpane.h"
#include "drawingpane_utils.h"
#include "graphicseditor_utils.h"
#include "graphicseditor_enum_types.h"

#include <math.h>

struct _DrawingPanePrivate
{
	GraphicsEditorWindow *window;

	gint cell_size;
	gint width, height;
	GList *figure_list;
	GList *editing_line;

	GtkDrawingArea *drawing_area;
	GtkScrolledWindow *scrolled_window;
};

static void drawing_pane_constructed(GObject *obj);
static void drawing_pane_finalize(GObject *obj);
static void drawing_pane_set_handlers(DrawingPane *pane);
static gboolean scrolled_window_scroll_event_handler(GtkScrolledWindow *widget, GdkEventScroll *event, gpointer user_data);
static gboolean drawing_area_scroll_event_handler(GtkWidget *drawing_area, GdkEventScroll *event, gpointer user_data);
static gboolean drawing_area_draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean drawing_area_configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean drawing_area_button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data);
static gboolean drawing_area_motion_notify_event_handler (GtkWidget *widget, GdkEventMotion  *event, gpointer data);
static void draw_pixel(cairo_t *cr, Pixel *pixel, DrawingPane *pane);
static void draw_figure(cairo_t *cr, GList *figure, DrawingPane *pane);
static void clear_figure(GList **figure);
static GraphicsEditorDrawingModeType get_drawing_mode(DrawingPane *pane);
static GList *get_line_figure(GraphicsEditorDrawingModeType drawing_mode, gint x1, gint y1, gint x2, gint y2);
static void move_line_end(GraphicsEditorDrawingModeType drawing_mode, GList **figure, gint x2, gint y2);
static void draw_net(cairo_t* cr, DrawingPane *pane);

G_DEFINE_TYPE_WITH_PRIVATE(DrawingPane, drawing_pane, GTK_TYPE_BIN);

static gboolean
scrolled_window_scroll_event_handler(GtkScrolledWindow *widget, GdkEventScroll *event, gpointer user_data)
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
			return FALSE;
		}

		gdouble delta_hadjustment_value = event->x / priv->cell_size;
		gdouble delta_vadjustment_value = event->y / priv->cell_size;

		GtkAdjustment *hadjustment = gtk_scrolled_window_get_hadjustment(widget);
		GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(widget);

		gdouble goal_hadjustment_value = gtk_adjustment_get_value(hadjustment) + delta_hadjustment_value * direction;
		gdouble goal_vadjustment_value = gtk_adjustment_get_value(vadjustment) + delta_vadjustment_value * direction;

		gtk_adjustment_set_value(hadjustment, goal_hadjustment_value);
		gtk_adjustment_set_value(vadjustment, goal_vadjustment_value);

		return TRUE;
	} else {
		return FALSE;
	}
}

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

		gint widget_width;
		gint widget_height;

		gtk_widget_get_size_request(GTK_WIDGET(priv->drawing_area), &widget_width, &widget_height);
		widget_width += direction * priv->width;
		widget_height += direction * priv->height;
		if (widget_width > 0 &&
				widget_height > 0 &&
				widget_width <= priv->width * 100 &&
				widget_height <= priv->height * 100) {
			gtk_widget_set_size_request(GTK_WIDGET(priv->drawing_area), widget_width, widget_height);
		} else {
			return TRUE;
		}
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
			| GDK_BUTTON_PRESS_MASK);

	g_signal_connect(priv->drawing_area,
			"scroll-event",
			G_CALLBACK(drawing_area_scroll_event_handler),
			pane);

	g_signal_connect(priv->scrolled_window,
			"scroll-event",
			G_CALLBACK(scrolled_window_scroll_event_handler),
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

}

static void
drawing_pane_finalize(GObject *obj)
{
	DrawingPanePrivate *priv;

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
			pixel->x * pane->priv->cell_size, pixel->y * pane->priv->cell_size,
			pane->priv->cell_size, pane->priv->cell_size);
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

gboolean
drawing_area_draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	DrawingPanePrivate *priv;
	GList *figure, *figure_list;

	priv = DRAWING_PANE(data)->priv;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, priv->width * priv->cell_size, priv->height * priv->cell_size);
	cairo_fill(cr);

	figure_list = priv->figure_list;
	while (figure_list != NULL) {
		figure = figure_list->data;
		draw_figure(cr, figure, DRAWING_PANE(data));
		figure_list = g_list_next(figure_list);
	}

	if (priv->editing_line != NULL) {
		draw_figure(cr, priv->editing_line, DRAWING_PANE(data));
	}

	draw_net(cr, DRAWING_PANE(data));
	return FALSE;
}

gboolean
drawing_area_configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	DrawingPanePrivate *priv;

	priv = DRAWING_PANE(data)->priv;

	gint drawing_area_width, drawing_area_height;
	gtk_widget_get_size_request(GTK_WIDGET(priv->drawing_area), &drawing_area_width, &drawing_area_height);

	priv->cell_size = drawing_area_width / priv->width;

	gtk_widget_queue_draw(GTK_WIDGET(priv->drawing_area));

	return FALSE;
}

gboolean
drawing_area_button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data)
{
	DrawingPanePrivate *priv;
	gint x, y;
	GraphicsEditorDrawingModeType drawing_mode;

	priv = DRAWING_PANE(data)->priv;
	switch (event->button) {
		case 1:
			drawing_mode = get_drawing_mode(DRAWING_PANE(data));
			x = floor(event->x / priv->cell_size);
			y = floor(event->y / priv->cell_size);
			if (priv->editing_line == NULL) {
				priv->editing_line = get_line_figure(drawing_mode, x, y, x, y);
			} else {
				move_line_end(drawing_mode, &priv->editing_line, x, y);

				if (priv->editing_line == NULL) {
					g_error("Editing line not NULL when drawing mode not line");
				}

				priv->figure_list = g_list_append(priv->figure_list, priv->editing_line);
				priv->editing_line = NULL;
			}
			break;
		case 3:
			clear_figure(&priv->editing_line);
			break;
		default:
			break;
	}
	gtk_widget_queue_draw(widget);

	return FALSE;
}

static void clear_figure(GList **figure) {
	Pixel *pixel;
	while (*figure != NULL) {
		pixel = (*figure)->data;
		*figure = g_list_remove(*figure, pixel);
		g_free(pixel);
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

	clear_figure(figure);

	*figure = get_line_figure(drawing_mode, x1, y1, x2, y2);
}

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
	Pixel *start_pixel;
	gint x1, y1, x2, y2;
	GraphicsEditorDrawingModeType drawing_mode;

	priv = DRAWING_PANE(data)->priv;

	if (priv->editing_line != NULL) {
		x2 = floor(event->x / priv->cell_size);
		y2 = floor(event->y / priv->cell_size);

		drawing_mode = get_drawing_mode(DRAWING_PANE(data));

		move_line_end(drawing_mode, &priv->editing_line, x2, y2);

		if (priv->editing_line == NULL) {
			g_error("Editing line not NULL when drawing mode not line");
		}

		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}
