#include "graphicseditorwin.h"
#include "graphicseditor.h"
#include "drawingpane.h"
#include "drawutils.h"

#include <gtk/gtk.h>

struct _GraphicsEditorWindow
{
	GtkApplicationWindow parent;
};

struct _GraphicsEditorWindowClass
{
	GtkApplicationWindowClass parent_class;
};

typedef	struct _GraphicsEditorWindowPrivate GraphicsEditorWindowPrivate;

struct _GraphicsEditorWindowPrivate
{
	DrawingPane *drawing_area;
	GtkToggleToolButton *tool_button_line_dda;
	GtkToggleToolButton *tool_button_line_bresenham;
	GtkToggleToolButton *tool_button_line_wu;
};

G_DEFINE_TYPE_WITH_PRIVATE(GraphicsEditorWindow, graphicseditor_window, GTK_TYPE_APPLICATION_WINDOW);

static void
graphicseditor_window_init (GraphicsEditorWindow *app)
{
	gtk_widget_init_template(GTK_WIDGET(app));
}

static void
graphicseditor_window_class_init (GraphicsEditorWindowClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/by/jylilov/graphicseditor/window.xml");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, tool_button_line_dda);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, tool_button_line_bresenham);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, tool_button_line_wu);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, drawing_area);
}

GraphicsEditorWindow *
graphicseditor_window_new (GraphicsEditor *app)
{
	return g_object_new (GRAPHICSEDITOR_WINDOW_TYPE, "application", app, NULL);
}

void graphicseditor_window_set_drawing_mode(GraphicsEditorWindow *win, gint mode) {
	GraphicsEditorWindowPrivate *priv;
	priv = graphicseditor_window_get_instance_private(win);

	gtk_toggle_tool_button_set_active(priv->tool_button_line_dda, mode == DRAWING_MODE_LINE_DDA);
	gtk_toggle_tool_button_set_active(priv->tool_button_line_wu, mode == DRAWING_MODE_LINE_WU);
	gtk_toggle_tool_button_set_active(priv->tool_button_line_bresenham, mode == DRAWING_MODE_LINE_BRESENHAM);

	gtk_widget_queue_draw(GTK_WIDGET(priv->drawing_area));

}

