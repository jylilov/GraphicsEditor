#include "drawingpane.h"

#include <gtk/gtk.h>

struct _DrawingPane
{
	GtkFrame parent;
};

struct _DrawingPaneClass
{
	GtkFrameClass parent_class;
};

typedef struct _DrawingPanePrivate DrawingPanePrivate;

struct _DrawingPanePrivate
{
	GtkLayout *drawing_area;
};

G_DEFINE_TYPE_WITH_PRIVATE(DrawingPane, drawing_pane, GTK_TYPE_FRAME);

static void
drawing_pane_init (DrawingPane *pane)
{
	gtk_widget_init_template(GTK_WIDGET(pane));

	/*DrawingPanePrivate *priv;

	priv = drawing_pane_get_instance_private(pane);

	gtk_widget_set_events (priv->drawing_area, gtk_widget_get_events (priv->drawing_area)
	                             | GDK_BUTTON_PRESS_MASK
	                             | GDK_POINTER_MOTION_MASK);*/
}

static void
drawing_pane_class_init (DrawingPaneClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
			"/by/jylilov/graphicseditor/drawing_pane.xml");
	//gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DrawingPane, drawing_area);
}

DrawingPane *
drawing_pane_new (void)
{
	return g_object_new (DRAWING_PANE_TYPE, NULL);
}
