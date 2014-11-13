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

void drawutils_set_drawing_mode(gint mode);

#endif /* __DRAWUTILS_H */
