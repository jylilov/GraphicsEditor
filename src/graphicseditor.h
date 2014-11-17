#ifndef __GRAPHICSEDITOR_H
#define __GRAPHICSEDITOR_H

#include <gtk/gtk.h>

#define GRAPHICSEDITOR_TYPE (graphicseditor_get_type ())
#define GRAPHICSEDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRAPHICSEDITOR_TYPE, GraphicsEditor))
#define GRAPHICSEDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GRAPHICSEDITOR_TYPE, GraphicsEditorClass))
#define IS_GRAPHICSEDITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GRAPHICSEDITOR_TYPE))
#define IS_GRAPHICSEDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GRAPHICSEDITOR_TYPE))
#define GRPAHICSEDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj) GRAPHICSEDITOR_TYPE, GraphicsEditorClass))

typedef struct _GraphicsEditor GraphicsEditor;
typedef struct _GraphicsEditorClass GraphicsEditorClass;
typedef struct _GraphicsEditorPrivate GraphicsEditorPrivate;

struct _GraphicsEditor
{
	GtkApplication parent;
	GraphicsEditorPrivate *priv;
};

struct _GraphicsEditorClass
{
	GtkApplicationClass parent_class;
};

GType graphicseditor_get_type (void);
GraphicsEditor *graphicseditor_new (void);

#endif /* __GRAPHICSEDITOR_H */
