#ifndef __GRAPHICSEDITORWIN_H
#define __GRAPHICSEDITORWIN_H

#include <gtk/gtk.h>

#include "graphicseditor.h"

#define GRAPHICSEDITOR_WINDOW_TYPE (graphicseditor_window_get_type ())
#define GRAPHICSEDITOR_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRAPHICSEDITOR_WINDOW_TYPE, GraphicsEditorWindow))

typedef struct _GraphicsEditorWindow GraphicsEditorWindow;
typedef struct _GraphicsEditorWindowClass GraphicsEditorWindowClass;

GType graphicseditor_window_get_type (void);
GraphicsEditorWindow *graphicseditor_window_new (GraphicsEditor *app);

#endif /* __GRAPHICSEDITORWIN_H */
