#ifndef __GRAPHICSEDITORWIN_H
#define __GRAPHICSEDITORWIN_H

#include "graphicseditor.h"

G_BEGIN_DECLS

#define GRAPHICSEDITOR_WINDOW_TYPE (graphicseditor_window_get_type ())
#define GRAPHICSEDITOR_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRAPHICSEDITOR_WINDOW_TYPE, GraphicsEditorWindow))
#define GRAPHICSEDITOR_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GRAPHICSEDITOR_WINDOW_TYPE, GraphicsEditorWindowClass))
#define GRAPHICSEDITOR_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GRAPHICSEDITOR_WINDOW_TYPE))
#define GRAPHICSEDITOR_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GRAPHICSEDITOR_WINDOW_TYPE))
#define GRPAHICSEDITOR_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj) GRAPHICSEDITOR_WINDOW_TYPE, GraphicsEditorWindowClass))

typedef struct _GraphicsEditorWindow GraphicsEditorWindow;
typedef struct _GraphicsEditorWindowClass GraphicsEditorWindowClass;
typedef	struct _GraphicsEditorWindowPrivate GraphicsEditorWindowPrivate;

struct _GraphicsEditorWindow
{
	GtkApplicationWindow parent;

	GraphicsEditorWindowPrivate *priv;
};

struct _GraphicsEditorWindowClass
{
	GtkApplicationWindowClass parent_class;
};

GType graphicseditor_window_get_type (void);
GraphicsEditorWindow *graphicseditor_window_new (GraphicsEditor *app);
void graphicseditor_window_set_drawing_mode(GraphicsEditorWindow *app, gint mode);

G_END_DECLS

#endif /* __GRAPHICSEDITORWIN_H */
