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
gboolean button_press_event_handler (GtkWidget *widget, GdkEventButton  *event, gpointer data);
gboolean motion_notify_event_handler (GtkWidget *widget, GdkEventMotion  *event, gpointer data);

void drawutils_set_drawing_mode(gint mode);\
gint drawutils_get_width();
gint drawutils_get_height();
gint drawutils_get_cell_size();


#endif /* __DRAWUTILS_H */
