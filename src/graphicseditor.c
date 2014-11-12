#include "graphicseditor.h"

#include <gtk/gtk.h>

#include "graphicseditorwin.h"

struct _GraphicsEditor
{
	GtkApplication parent;
};

struct _GraphicsEditorClass
{
	GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(GraphicsEditor, graphicseditor, GTK_TYPE_APPLICATION);

static void
graphicseditor_init (GraphicsEditor *app)
{
}

static void
graphicseditor_activate (GApplication *app)
{
	GraphicsEditorWindow *win;

	win = graphicseditor_window_new (GRAPHICSEDITOR (app));

	gtk_window_present (GTK_WINDOW (win));

	set_graphicsedior_window(win);
}

static void
graphicseditor_class_init (GraphicsEditorClass *class)
{
	G_APPLICATION_CLASS (class)->activate = graphicseditor_activate;
}

GraphicsEditor *
graphicseditor_new (void)
{
	return g_object_new (GRAPHICSEDITOR_TYPE,
			"application-id", "by.jylilov.grapheditor",
			"flags", G_APPLICATION_HANDLES_OPEN,
			NULL);
}
