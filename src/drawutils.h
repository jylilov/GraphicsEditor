#ifndef __DRAWUTILS_H
#define __DRAWUTILS_H

#include "graphicseditorwin.h"

#include <gtk/gtk.h>

#define DRAWING_MODE_NONE 0
#define DRAWING_MODE_LINE_DDA 1
#define DRAWING_MODE_LINE_BRESENHAM 2
#define DRAWING_MODE_LINE_WU 3

gboolean draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data);

void set_drawing_mode(gint mode);
void set_graphicsedior_window(GraphicsEditorWindow *ge_window);

#endif /* __DRAWUTILS_H */
