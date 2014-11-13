#include "graphicseditor.h"
#include "graphicseditorwin.h"
#include "drawutils.h"

#include <gtk/gtk.h>


struct _GraphicsEditor
{
	GtkApplication parent;
	GraphicsEditorWindow *window;
};

struct _GraphicsEditorClass
{
	GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(GraphicsEditor, graphicseditor, GTK_TYPE_APPLICATION);

static void set_drawing_mode_line_dda(GSimpleAction *action, GVariant *parameter, gpointer app);
static void set_drawing_mode_line_bresenham(GSimpleAction *action, GVariant *parameter, gpointer app);
static void set_drawing_mode_line_wu(GSimpleAction *action, GVariant *parameter, gpointer app);

static GActionEntry app_tool_entries[] =
{
		{ "drawing-mode.line-dda", set_drawing_mode_line_dda },
		{ "drawing-mode.line-bresenham", set_drawing_mode_line_bresenham },
		{ "drawing-mode.line-wu", set_drawing_mode_line_wu }
};

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

	GRAPHICSEDITOR(app)->window = win;
}

static void
set_tools_action_enabled(GraphicsEditor *ge, gboolean enabled) {
	if (enabled == FALSE) {
		gint i;
		for (i = 0; i < G_N_ELEMENTS(app_tool_entries); ++i) {
			g_action_map_remove_action(G_ACTION_MAP(ge), app_tool_entries[i].name);
		}
	} else {
		g_action_map_add_action_entries(G_ACTION_MAP(ge),
					app_tool_entries, G_N_ELEMENTS(app_tool_entries), ge);
	}
}

static void
set_drawing_mode_line_dda(GSimpleAction *action,
		GVariant *parameter,
		gpointer app)
{
	set_tools_action_enabled(app, FALSE);
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->window, DRAWING_MODE_LINE_DDA);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_DDA);
}

static void
set_drawing_mode_line_bresenham(GSimpleAction *action,
		GVariant *parameter,
		gpointer app)
{
	set_tools_action_enabled(app, FALSE);
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->window, DRAWING_MODE_LINE_BRESENHAM);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_BRESENHAM);
}

static void
set_drawing_mode_line_wu(GSimpleAction *action,
		GVariant *parameter,
		gpointer app)
{
	set_tools_action_enabled(app, FALSE);
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->window, DRAWING_MODE_LINE_WU);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_WU);
}

static void
graphicseditor_startup (GApplication *app)
{
	G_APPLICATION_CLASS(graphicseditor_parent_class)->startup(app);

	g_action_map_add_action_entries(G_ACTION_MAP(app),
			app_tool_entries, G_N_ELEMENTS(app_tool_entries), app);
}

static void
graphicseditor_class_init (GraphicsEditorClass *class)
{
	G_APPLICATION_CLASS(class)->activate = graphicseditor_activate;
	G_APPLICATION_CLASS(class)->startup = graphicseditor_startup;
}

GraphicsEditor *
graphicseditor_new (void)
{
	return g_object_new (GRAPHICSEDITOR_TYPE,
			"application-id", "by.jylilov.grapheditor",
			"flags", G_APPLICATION_HANDLES_OPEN,
			NULL);
}
