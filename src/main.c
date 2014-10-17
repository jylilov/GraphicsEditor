#include <gtk/gtk.h>

#include "graphicseditor.h"

int
main (int argc, char *argv[])
{
	return g_application_run (G_APPLICATION (graphicseditor_new()), argc, argv);
}
