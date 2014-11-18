#ifndef __DRAWINGPANE_H
#define __DRAWINGPANE_H

#include <gtk/gtk.h>

#define DRAWING_PANE_TYPE (drawing_pane_get_type ())
#define DRAWIND_PANE_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), DRAWING_PANE_TYPE, DrawingPane))

typedef struct _DrawingPane DrawingPane;
typedef struct _DrawingPaneClass DrawingPaneClass;

GType drawing_pane_get_type (void);
DrawingPane *drawing_pane_new (void);

#endif /* __DRAWINGPANE_H */
