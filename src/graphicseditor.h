#ifndef __GRAPHICSEDITOR_H
#define __GRAPHICSEDITOR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GRAPHICSEDITOR_TYPE (graphicseditor_get_type ())
#define GRAPHICSEDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRAPHICSEDITOR_TYPE, GraphicsEditor))
#define GRAPHICSEDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GRAPHICSEDITOR_TYPE, GraphicsEditorClass))
#define GRAPHICSEDITOR_IS_INSTANCE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GRAPHICSEDITOR_TYPE))
#define GRAPHICSEDITOR_IS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), GRAPHICSEDITOR_TYPE))
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

G_END_DECLS

#endif /* __GRAPHICSEDITOR_H */
