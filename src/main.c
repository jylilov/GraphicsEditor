#include <gtk/gtk.h>

#include "graphicseditor.h"

int
main (int argc, char *argv[])
{
	GraphicsEditor *app;
	int status;

	app = graphicseditor_new();
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref(app);

	return status;
}
