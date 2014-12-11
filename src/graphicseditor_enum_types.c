
/* Generated data (by glib-mkenums) */

#include "graphicseditor_enum_types.h"

/* enumerations from "graphicseditor_utils.h" */
#include "graphicseditor_utils.h"

GType
graphics_editor_drawing_mode_type_get_type (void)
{
	static GType the_type = 0;

	if (the_type == 0)
	{
		static const GEnumValue values[] = {
			{ GRAPHICSEDITOR_DRAWING_MODE_NONE,
			  "GRAPHICSEDITOR_DRAWING_MODE_NONE",
			  "none" },
			{ GRAPHICSEDITOR_DRAWING_MODE_DDA_LINE,
			  "GRAPHICSEDITOR_DRAWING_MODE_DDA_LINE",
			  "dda-line" },
			{ GRAPHICSEDITOR_DRAWING_MODE_BRESENHAM_LINE,
			  "GRAPHICSEDITOR_DRAWING_MODE_BRESENHAM_LINE",
			  "bresenham-line" },
			{ GRAPHICSEDITOR_DRAWING_MODE_WU_LINE,
			  "GRAPHICSEDITOR_DRAWING_MODE_WU_LINE",
			  "wu-line" },
			{ GRAPHICSEDITOR_DRAWING_MODE_HYPERBOLE,
			  "GRAPHICSEDITOR_DRAWING_MODE_HYPERBOLE",
			  "hyperbole" },
			{ GRAPHICSEDITOR_DRAWING_MODE_ELLIPSE,
			  "GRAPHICSEDITOR_DRAWING_MODE_ELLIPSE",
			  "ellipse" },
			{ GRAPHICSEDITOR_DRAWING_MODE_HERMIT,
			  "GRAPHICSEDITOR_DRAWING_MODE_HERMIT",
			  "hermit" },
			{ GRAPHICSEDITOR_DRAWING_MODE_BEZIER,
			  "GRAPHICSEDITOR_DRAWING_MODE_BEZIER",
			  "bezier" },
			{ GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE,
			  "GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE",
			  "b-spline" },
			{ 0, NULL, NULL }
		};
		the_type = g_enum_register_static (
				g_intern_static_string ("GraphicsEditorDrawingModeType"),
				values);
	}
	return the_type;
}


/* Generated data ends here */

