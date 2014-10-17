#ifndef __GRAPHICSEDITOR_H
#define __GRAPHICSEDITOR_H

#include <gtk/gtk.h>

#define GRAPHICSEDITOR_TYPE (graphicseditor_get_type ())
#define GRAPHICSEDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRAPHICSEDITOR_TYPE, GraphicsEditor))

typedef struct _GraphicsEditor GraphicsEditor;
typedef struct _GraphicsEditorClass GraphicsEditorClass;

GType graphicseditor_get_type (void);
GraphicsEditor *graphicseditor_new (void);

#endif /* __GRAPHICSEDITOR_H */
