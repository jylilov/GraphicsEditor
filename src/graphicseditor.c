#include "graphicseditor.h"

#include "drawingpane_utils.h"
#include "graphicseditorwin.h"

struct _GraphicsEditorPrivate {
	GraphicsEditorWindow *window;

	GSettings *settings;
	GSimpleAction *drawing_mode;
};

static void graphicseditor_activate(GApplication *app);
static void graphicseditor_startup(GApplication *app);
static void graphicseditor_finalize(GObject *obj);
static void graphicseditor_set_accelerator(GraphicsEditor *app);
static void graphicseditor_set_actions(GraphicsEditor *app);
static void graphicseditor_set_app_menu(GraphicsEditor *app);
static void graphicseditor_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void graphicseditor_about(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void graphicseditor_changed_drawing_mode(GSettings *setting, GVariant *parameter, gpointer user_data);
static void graphicseditor_change_drawing_mode(GSimpleAction *action, GVariant *parameter, gpointer user_data);

G_DEFINE_TYPE_WITH_PRIVATE(GraphicsEditor, graphicseditor, GTK_TYPE_APPLICATION);

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

	g_settings_bind(priv->settings,
			"drawing-mode",
			priv->window,
			"drawing-mode",
			G_SETTINGS_BIND_SET | G_SETTINGS_BIND_GET);
}

static void
graphicseditor_startup (GApplication *app)
{
	G_APPLICATION_CLASS(graphicseditor_parent_class)->startup(app);

	graphicseditor_set_accelerator(GRAPHICSEDITOR(app));
	graphicseditor_set_actions(GRAPHICSEDITOR(app));
	graphicseditor_set_app_menu(GRAPHICSEDITOR(app));
}

static void
graphicseditor_finalize(GObject *obj)
{
	GraphicsEditorPrivate *priv;
	priv = GRAPHICSEDITOR(obj)->priv;

	g_clear_object (&(priv->settings));
	g_clear_object (&(priv->drawing_mode));

	if (G_OBJECT_CLASS (graphicseditor_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (graphicseditor_parent_class)->finalize(obj);
}

static void
graphicseditor_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GApplication *app;

	g_return_if_fail(GRAPHICSEDITOR_IS_INSTANCE(user_data));
	app = G_APPLICATION (user_data);

	g_application_quit (app);
}

static void
graphicseditor_about(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GraphicsEditor *app;

	g_return_if_fail(GRAPHICSEDITOR_IS_INSTANCE(user_data));
	app = GRAPHICSEDITOR(user_data);

	const gchar *comments = "This program is simple graphics editor.";

	const gchar *authors[] = {
		"Alexander Jylilov <alexander.jylilov@gmail.com>",
		NULL
	};

	gtk_show_about_dialog (GTK_WINDOW (app->priv->window),
			"program-name", "Graphics Editor",
			"website", "https://github.com/jylilov/GraphicsEditor",
			"website-label", "Project on GitHub",
			"comments", comments,
			"authors", authors,
			"artists", authors,
			NULL);

}

static void
graphicseditor_set_accelerator(GraphicsEditor *app)
{
	GVariant *va;

	gtk_application_add_accelerator (GTK_APPLICATION (app), "<Primary>q", "app.quit", NULL);
	gtk_application_add_accelerator (GTK_APPLICATION (app), "F7", "app.about", NULL);

	va = g_variant_new_string("none");
	gtk_application_add_accelerator (GTK_APPLICATION (app), "0", "app.drawing-mode", va);
	g_variant_unref(va);

	va = g_variant_new_string("dda-line");
	gtk_application_add_accelerator (GTK_APPLICATION (app), "1", "app.drawing-mode", va);
	g_variant_unref(va);

	va = g_variant_new_string("bresenham-line");
	gtk_application_add_accelerator (GTK_APPLICATION (app), "2", "app.drawing-mode", va);
	g_variant_unref(va);

	va = g_variant_new_string("wu-line");
	gtk_application_add_accelerator (GTK_APPLICATION (app), "3", "app.drawing-mode", va);
	g_variant_unref(va);

	va = g_variant_new_string("hyperbole");
	gtk_application_add_accelerator (GTK_APPLICATION (app), "4", "app.drawing-mode", va);
	g_variant_unref(va);

}

static void
graphicseditor_set_actions(GraphicsEditor *app)
{
	GSimpleAction *quit;
	GSimpleAction *about;

	quit = g_simple_action_new("quit", NULL);
	g_signal_connect(quit,
			"activate",
			G_CALLBACK(graphicseditor_quit),
			app);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(quit));
	g_object_unref(quit);

	about = g_simple_action_new("about", NULL);
	g_signal_connect(about,
			"activate",
			G_CALLBACK(graphicseditor_about),
			app);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(about));
	g_object_unref(about);

	app->priv->drawing_mode = g_simple_action_new_stateful(
			"drawing-mode",
			G_VARIANT_TYPE_STRING,
			g_settings_get_value(app->priv->settings, "drawing-mode"));

	g_signal_connect(app->priv->drawing_mode,
			"activate",
			G_CALLBACK(graphicseditor_change_drawing_mode),
			app);

	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app->priv->drawing_mode));

}

static void
graphicseditor_set_app_menu(GraphicsEditor *app)
{
	GMenu *menu;
	GMenu *submenu;
	GMenu *section;

	menu = g_menu_new();

	submenu = g_menu_new();

	section = g_menu_new();
	g_menu_append(section, "New", "app.new");
	g_menu_append(section, "Close", "app.close");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);

	section = g_menu_new();
	g_menu_append(section, "Quit", "app.quit");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);

	g_menu_append_submenu(menu, "File", G_MENU_MODEL(submenu));
	g_object_unref(submenu);

	submenu = g_menu_new();

	section = g_menu_new();
	g_menu_append(section, "None", "app.drawing-mode::none");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);

	section = g_menu_new();
	g_menu_append(section, "DDA-line", "app.drawing-mode::dda-line");
	g_menu_append(section, "Bresenham's line", "app.drawing-mode::bresenham-line");
	g_menu_append(section, "Wu's line", "app.drawing-mode::wu-line");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);

	section = g_menu_new();
	g_menu_append(section, "Hyperbole", "app.drawing-mode::hyperbole");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);

	g_menu_append_submenu(menu, "Drawing mode", G_MENU_MODEL(submenu));
	g_object_unref(submenu);

	submenu = g_menu_new();
	g_menu_append_submenu(menu, "Window", G_MENU_MODEL(submenu));
	g_object_unref(submenu);

	submenu = g_menu_new();
	g_menu_append(submenu, "About", "app.about");
	g_menu_append_submenu(menu, "Help", G_MENU_MODEL(submenu));
	g_object_unref(submenu);

	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu));
	g_object_unref(menu);
}

static void
graphicseditor_changed_drawing_mode(GSettings *setting, GVariant *parameter, gpointer user_data)
{
	GraphicsEditorPrivate *priv;

	g_return_if_fail(GRAPHICSEDITOR_IS_INSTANCE(user_data));
	priv = GRAPHICSEDITOR(user_data)->priv;

	g_simple_action_set_state(priv->drawing_mode,
			g_settings_get_value(priv->settings, "drawing-mode"));
}

static void
graphicseditor_change_drawing_mode(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GraphicsEditorPrivate *priv;

	g_return_if_fail(GRAPHICSEDITOR_IS_INSTANCE(user_data));
	priv = GRAPHICSEDITOR(user_data)->priv;

	g_settings_set_value(priv->settings, "drawing-mode", parameter);
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
