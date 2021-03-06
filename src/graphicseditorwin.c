#include <glib-unix.h>
#include "graphicseditorwin.h"
#include "graphicseditor_enum_types.h"
#include "graphicseditor_utils.h"
#include "drawingpane.h"

struct _GraphicsEditorWindowPrivate
{
	DrawingPane *drawing_area;
	GtkToolPalette *tool_palette;
	GtkFrame *working_area;
	GraphicsEditorDrawingModeType drawing_mode;
	GtkLabel *statusbar;
};

enum
{
  PROP_DRAWING_MODE = 1
};

static void graphicseditor_window_constructed(GObject *object);
static void graphicseditor_window_finalize(GObject *object);
static void graphicseditor_window_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void graphicseditor_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void graphicseditor_window_set_toolpalette(GraphicsEditorWindow *win);
static void graphicseditor_window_cursor_changed(GObject *object, GParamSpec *spec, gpointer user_data);


G_DEFINE_TYPE_WITH_PRIVATE(GraphicsEditorWindow, graphicseditor_window, GTK_TYPE_APPLICATION_WINDOW);

static void
graphicseditor_window_init (GraphicsEditorWindow *win)
{
	gtk_widget_init_template(GTK_WIDGET(win));

	win->priv = graphicseditor_window_get_instance_private(win);
}

static void
graphicseditor_window_class_init (GraphicsEditorWindowClass *class)
{
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(class);

	object_class->constructed = graphicseditor_window_constructed;
	object_class->finalize = graphicseditor_window_finalize;
	object_class->set_property = graphicseditor_window_set_property;
	object_class->get_property = graphicseditor_window_get_property;

	g_object_class_install_property(
			object_class,
			PROP_DRAWING_MODE,
			g_param_spec_enum(
					"drawing-mode",
					"Drawing mode",
					"Selected drawing mode, eg: dda-line, hyperbole, etc.",
					GRAPHICS_EDITOR_DRAWING_MODE_TYPE,
					GRAPHICSEDITOR_DRAWING_MODE_NONE,
					G_PARAM_READWRITE));

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/by/jylilov/graphicseditor/window.xml");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, tool_palette);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, working_area);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, statusbar);
}

static void
graphicseditor_window_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GraphicsEditorWindowPrivate *priv;

	g_return_if_fail(GRAPHICSEDITOR_WINDOW(object));
	priv = (GRAPHICSEDITOR_WINDOW(object))->priv;

	switch (property_id) {
	case PROP_DRAWING_MODE:
		priv->drawing_mode = g_value_get_enum(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
graphicseditor_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GraphicsEditorWindowPrivate *priv;

	g_return_if_fail(GRAPHICSEDITOR_WINDOW(object));
	priv = (GRAPHICSEDITOR_WINDOW(object))->priv;

	switch (property_id) {
	case PROP_DRAWING_MODE:
		g_value_set_enum(value, priv->drawing_mode);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
graphicseditor_window_set_toolpalette(GraphicsEditorWindow *win)
{
	GtkToolItemGroup *group;
	GtkToolItem *item;
	gint i;

	group = GTK_TOOL_ITEM_GROUP(gtk_tool_item_group_new("Lines"));
	i = 0;

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "DDA-line algorithm", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::dda-line");
	gtk_tool_item_group_insert(group, item, i++);

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Bresenham's line algorithm", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::bresenham-line");
	gtk_tool_item_group_insert(group, item, i++);

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Xiaolin Wu's line algorithm", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::wu-line");
	gtk_tool_item_group_insert(group, item, i++);

	gtk_container_add(GTK_CONTAINER(win->priv->tool_palette), GTK_WIDGET(group));

	group = GTK_TOOL_ITEM_GROUP(gtk_tool_item_group_new("Lines of the 2nd order"));
	i = 0;

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Hyperbole", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::hyperbole");
	gtk_tool_item_group_insert(group, item, i++);

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Ellipse", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::ellipse");
	gtk_tool_item_group_insert(group, item, i++);

	gtk_container_add(GTK_CONTAINER(win->priv->tool_palette), GTK_WIDGET(group));

	group = GTK_TOOL_ITEM_GROUP(gtk_tool_item_group_new("Interpolation and antialiasing"));
	i = 0;

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Hermitian form", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::hermit");
	gtk_tool_item_group_insert(group, item, i++);

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Bezier form", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::bezier");
	gtk_tool_item_group_insert(group, item, i++);

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "B-spline", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::b-spline");
	gtk_tool_item_group_insert(group, item, i++);

	gtk_container_add(GTK_CONTAINER(win->priv->tool_palette), GTK_WIDGET(group));

	gtk_widget_show_all(GTK_WIDGET(win->priv->tool_palette));
}

static void
graphicseditor_window_constructed(GObject *object)
{
	GraphicsEditorWindow *win;
	GraphicsEditorWindowPrivate *priv;

	if (G_OBJECT_CLASS (graphicseditor_window_parent_class)->constructed != NULL)
		G_OBJECT_CLASS (graphicseditor_window_parent_class)->constructed (object);

	win = GRAPHICSEDITOR_WINDOW(object);
	priv = win->priv;

	graphicseditor_window_set_toolpalette(GRAPHICSEDITOR_WINDOW(object));

	// TODO many drawing panes
	priv->drawing_area = drawing_pane_new(win);
	gtk_container_add(GTK_CONTAINER(priv->working_area), GTK_WIDGET(priv->drawing_area));

	g_signal_connect(priv->drawing_area,
			"notify::cursor-x",
			G_CALLBACK(graphicseditor_window_cursor_changed),
			win);

	g_signal_connect(priv->drawing_area,
			"notify::cursor-y",
			G_CALLBACK(graphicseditor_window_cursor_changed),
			win);

	gtk_widget_show_all(GTK_WIDGET(priv->drawing_area));
}

static void
graphicseditor_window_finalize(GObject *object)
{
	GraphicsEditorWindowPrivate *priv;
	priv = GRAPHICSEDITOR_WINDOW(object)->priv;

	if (G_OBJECT_CLASS (graphicseditor_window_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (graphicseditor_window_parent_class)->finalize(object);
}

static void graphicseditor_window_cursor_changed(GObject *object, GParamSpec *spec, gpointer user_data)
{
	GraphicsEditorWindowPrivate *priv;

	priv = GRAPHICSEDITOR_WINDOW(user_data)->priv;

	gint x, y;

	g_object_get(priv->drawing_area,
			"cursor-x", &x,
			"cursor-y", &y,
			NULL
	);

	gtk_label_set_label(GTK_LABEL(priv->statusbar), g_strdup_printf("Coordinates: %d, %d", x, y));
}


GraphicsEditorWindow *
graphicseditor_window_new (GraphicsEditor *app)
{
	return g_object_new (GRAPHICSEDITOR_WINDOW_TYPE,
			"application", app,
			NULL);
}
