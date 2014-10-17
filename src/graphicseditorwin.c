#include "graphicseditorwin.h"

#include <gtk/gtk.h>
#include "graphicseditor.h"

struct _GraphicsEditorWindow
{
	GtkApplicationWindow parent;
};

struct _GraphicsEditorWindowClass
{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(GraphicsEditorWindow, graphicseditor_window, GTK_TYPE_APPLICATION_WINDOW);

static void
graphicseditor_window_init (GraphicsEditorWindow *app)
{
	gtk_widget_init_template(GTK_WIDGET(app));
}

static void
graphicseditor_window_class_init (GraphicsEditorWindowClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
			"/by/jylilov/grapheditor/window.ui");
}

GraphicsEditorWindow *
graphicseditor_window_new (GraphicsEditor *app)
{
	return g_object_new (GRAPHICSEDITOR_WINDOW_TYPE, "application", app, NULL);
}
