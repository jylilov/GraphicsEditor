#ifndef __DRAWUTILS_H
#define __DRAWUTILS_H

#include <gtk/gtk.h>

gboolean draw_handler (GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean configure_event_handler (GtkWidget *widget, GdkEventConfigure *event, gpointer data);

#endif /* __DRAWUTILS_H */
