#include "graphicseditor.h"
#include "graphicseditorwin.h"
#include "drawutils.h"

#include <gtk/gtk.h>

struct _GraphicsEditorPrivate {
	GraphicsEditorWindow *window;

	GSettings *settings;
	GSimpleAction *drawing_mode;
};

static void graphicseditor_activate(GApplication *app);
static void graphicseditor_startup(GApplication *app);
static void graphicseditor_finalize(GObject *obj);
static void graphicseditor_set_app_menu(GraphicsEditor *app);
static void graphicseditor_changed_drawing_mode(GSettings *setting, GVariant *parameter, gpointer user_data);
static void graphicseditor_change_drawing_mode(GSimpleAction *action, GVariant *parameter, gpointer user_data);

G_DEFINE_TYPE_WITH_PRIVATE(GraphicsEditor, graphicseditor, GTK_TYPE_APPLICATION);

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
	app->priv = graphicseditor_get_instance_private(app);
}

static void
graphicseditor_class_init (GraphicsEditorClass *class)
{
	GApplicationClass *application_class;
	GObjectClass *object_class;

	application_class = G_APPLICATION_CLASS(class);
	application_class->activate = graphicseditor_activate;
	application_class->startup = graphicseditor_startup;

	object_class = G_OBJECT_CLASS(class);
	object_class->finalize = graphicseditor_finalize;
}

static void
graphicseditor_activate (GApplication *app)
{
	GraphicsEditorPrivate *priv;
	priv = GRAPHICSEDITOR(app)->priv;

	priv->window = graphicseditor_window_new (GRAPHICSEDITOR (app));
	gtk_window_present (GTK_WINDOW (priv->window));
}

static void
graphicseditor_startup (GApplication *app)
{
	GraphicsEditorPrivate *priv;
	priv = GRAPHICSEDITOR(app)->priv;

	G_APPLICATION_CLASS(graphicseditor_parent_class)->startup(app);

	graphicseditor_set_app_menu(GRAPHICSEDITOR(app));

	g_object_set (gtk_settings_get_default (),
			"gtk-application-prefer-dark-theme", TRUE,
			NULL);
}

static void
graphicseditor_finalize(GObject *obj)
{
	GraphicsEditorPrivate *priv;
	priv = GRAPHICSEDITOR(obj)->priv;

	g_clear_object (&(priv->settings));

	if (G_OBJECT_CLASS (graphicseditor_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (graphicseditor_parent_class)->finalize(obj);
}

static void
graphicseditor_set_app_menu(GraphicsEditor *app)
{
	app->priv->drawing_mode = g_simple_action_new_stateful(
			"drawing-mode",
			G_VARIANT_TYPE_STRING,
			g_settings_get_value(app->priv->settings, "drawing-mode"));

	g_signal_connect(app->priv->drawing_mode,
			"activate",
			G_CALLBACK(graphicseditor_change_drawing_mode),
			app);

	g_action_map_add_action_entries(G_ACTION_MAP(app),
				app_tool_entries, G_N_ELEMENTS(app_tool_entries), app);
}

static void
graphicseditor_changed_drawing_mode(GSettings *setting, GVariant *parameter, gpointer user_data)
{
	GraphicsEditorPrivate *priv;

	g_return_if_fail(IS_GRAPHICSEDITOR(user_data));
	priv = GRAPHICSEDITOR(user_data)->priv;

	g_simple_action_set_state(priv->drawing_mode,
			g_settings_get_value(priv->settings, "drawing-mode"));
}

static void
graphicseditor_change_drawing_mode(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GraphicsEditorPrivate *priv;

	g_return_if_fail(IS_GRAPHICSEDITOR(user_data));
	priv = GRAPHICSEDITOR(user_data)->priv;

	g_settings_set_value(priv->settings, "drawing-mode", parameter);
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
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->priv->window, DRAWING_MODE_LINE_DDA);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_DDA);
}

static void
set_drawing_mode_line_bresenham(GSimpleAction *action,
		GVariant *parameter,
		gpointer app)
{
	set_tools_action_enabled(app, FALSE);
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->priv->window, DRAWING_MODE_LINE_BRESENHAM);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_BRESENHAM);
}

static void
set_drawing_mode_line_wu(GSimpleAction *action,
		GVariant *parameter,
		gpointer app)
{
	set_tools_action_enabled(app, FALSE);
	graphicseditor_window_set_drawing_mode(GRAPHICSEDITOR(app)->priv->window, DRAWING_MODE_LINE_WU);
	set_tools_action_enabled(app, TRUE);

	drawutils_set_drawing_mode(DRAWING_MODE_LINE_WU);
}

GraphicsEditor *
graphicseditor_new (void)
{
	GraphicsEditor *app;

	g_set_application_name("Graphics Editor");

	app = g_object_new (GRAPHICSEDITOR_TYPE,
			"application-id", "by.jylilov.graphicseditor",
			NULL);

	app->priv->settings = g_settings_new("by.jylilov.graphicseditor");

	g_signal_connect(app->priv->settings,
			"changed::drawing-mode",
			G_CALLBACK(graphicseditor_changed_drawing_mode),
			app);

	return app;
}
