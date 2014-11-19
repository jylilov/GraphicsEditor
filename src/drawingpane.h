#ifndef __DRAWINGPANE_H
#define __DRAWINGPANE_H

#include "graphicseditorwin.h"

G_BEGIN_DECLS

#define DRAWING_PANE_DEFAULT_WIDTH 800
#define DRAWING_PANE_DEFAULT_HEIGHT 600
#define DRAWING_PANE_TYPE (drawing_pane_get_type ())
#define DRAWING_PANE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), DRAWING_PANE_TYPE, DrawingPane))

typedef struct _DrawingPane DrawingPane;
typedef struct _DrawingPaneClass DrawingPaneClass;
typedef struct _DrawingPanePrivate DrawingPanePrivate;

struct _DrawingPane
{
	GtkBin parent;
	DrawingPanePrivate *priv;
};

struct _DrawingPaneClass
{
	GtkBinClass parent_class;
};

GType drawing_pane_get_type (void);
DrawingPane *drawing_pane_new (GraphicsEditorWindow *win);
DrawingPane *drawing_pane_new_with_size (GraphicsEditorWindow *win, gint width, gint height);

G_END_DECLS

#endif /* __DRAWINGPANE_H */
