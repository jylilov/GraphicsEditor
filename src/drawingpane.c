#include "drawingpane.h"
#include "drawutils.h"

#include <gtk/gtk.h>
#include <math.h>

struct _DrawingPane
{
	GtkBin parent;
};

struct _DrawingPaneClass
{
	GtkBinClass parent_class;
};

typedef struct _DrawingPanePrivate DrawingPanePrivate;

struct _DrawingPanePrivate
{
	GtkDrawingArea *drawing_area;
	GtkScrolledWindow *scrolled_window;
};

G_DEFINE_TYPE_WITH_PRIVATE(DrawingPane, drawing_pane, GTK_TYPE_BIN);

static gboolean
scrolled_window_scroll_event_handler(GtkScrolledWindow *widget, GdkEventScroll *event, gpointer user_data)
{
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

		gdouble delta_hadjustment_value = event->x / drawutils_get_cell_size();
		gdouble delta_vadjustment_value = event->y / drawutils_get_cell_size();

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
drawing_area_scroll_event_handler(GtkWidget *drawing_area, GdkEventScroll *event, gpointer user_data)
{
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

		gint width;
		gint height;

		gint pane_width = drawutils_get_width();
		gint pane_height = drawutils_get_height();

		gtk_widget_get_size_request(GTK_WIDGET(drawing_area), &width, &height);
		width += direction * drawutils_get_width();
		height += direction * drawutils_get_height();
		if (width > 0 && height > 0 && width <= pane_height * 50 &&  height <= pane_height * 50) {
			gtk_widget_set_size_request(GTK_WIDGET(drawing_area), width, height);
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

	DrawingPanePrivate *priv;

	priv = drawing_pane_get_instance_private(pane);

	drawutils_init(priv->drawing_area, 400, 300);

	gtk_widget_set_valign(GTK_WIDGET(priv->drawing_area), GTK_ALIGN_CENTER);
	gtk_widget_set_halign(GTK_WIDGET(priv->drawing_area), GTK_ALIGN_CENTER);

	gtk_widget_set_events (GTK_WIDGET(priv->drawing_area), gtk_widget_get_events (GTK_WIDGET(priv->drawing_area))
			| GDK_SCROLL_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_BUTTON_PRESS_MASK);
}

static void
drawing_pane_class_init (DrawingPaneClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
			"/by/jylilov/graphicseditor/drawing_pane.xml");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DrawingPane, drawing_area);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DrawingPane, scrolled_window);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), draw_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), configure_event_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), drawing_area_scroll_event_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), scrolled_window_scroll_event_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), button_press_event_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), motion_notify_event_handler);
}

DrawingPane *
drawing_pane_new (void)
{
	return g_object_new (DRAWING_PANE_TYPE, NULL);
}
