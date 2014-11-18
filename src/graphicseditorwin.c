#include "graphicseditorwin.h"
#include "graphicseditor_enum_types.h"
#include "graphicseditor_utils.h"
#include "drawingpane.h"
#include "drawutils.h"

struct _GraphicsEditorWindowPrivate
{
	DrawingPane *drawing_area;
	GtkToolPalette *tool_palette;
	GraphicsEditorDrawingModeType drawing_mode;
};

enum
{
  PROP_DRAWING_MODE = 1
};

static void graphicseditor_window_constructed(GObject *object);
static void graphicseditor_window_finalize(GObject *object);
static void graphicseditor_window_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void graphicseditor_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

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
					GRAPHICSEDITOR_DRAWING_MODE_TYPE,
					GRAPHICSEDITOR_DRAWING_MODE_NONE,
					G_PARAM_READWRITE));

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/by/jylilov/graphicseditor/window.xml");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, tool_palette);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), GraphicsEditorWindow, drawing_area);
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
		drawutils_set_drawing_mode(priv->drawing_mode);
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
graphicseditor_window_constructed(GObject *object)
{
	GraphicsEditorWindowPrivate *priv;
	GtkToolItemGroup *group;
	GtkToolItem *item;
	gint i;

	if (G_OBJECT_CLASS (graphicseditor_window_parent_class)->constructed != NULL)
		G_OBJECT_CLASS (graphicseditor_window_parent_class)->constructed (object);

	priv = GRAPHICSEDITOR_WINDOW(object)->priv;

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

	gtk_container_add(GTK_CONTAINER(priv->tool_palette), GTK_WIDGET(group));

	group = GTK_TOOL_ITEM_GROUP(gtk_tool_item_group_new("Lines of the 2nd order"));
	i = 0;

	item = gtk_toggle_tool_button_new();
	g_object_set(item, "label", "Hyperbole", NULL);
	gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(item), "app.drawing-mode::hyperbole");
	gtk_tool_item_group_insert(group, item, i++);

	gtk_container_add(GTK_CONTAINER(priv->tool_palette), GTK_WIDGET(group));

	gtk_widget_show_all(GTK_WIDGET(priv->tool_palette));
}

static void
graphicseditor_window_finalize(GObject *object)
{
	GraphicsEditorWindowPrivate *priv;
	priv = GRAPHICSEDITOR_WINDOW(object)->priv;

	if (G_OBJECT_CLASS (graphicseditor_window_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (graphicseditor_window_parent_class)->finalize(object);
}

GraphicsEditorWindow *
graphicseditor_window_new (GraphicsEditor *app)
{
	return g_object_new (GRAPHICSEDITOR_WINDOW_TYPE,
			"application", app,
			NULL);
}
