#ifndef GRAPHICSEDITOR_UTILS_H_
#define GRAPHICSEDITOR_UTILS_H_

G_BEGIN_DECLS

typedef enum
{
  GRAPHICSEDITOR_DRAWING_MODE_NONE,
  GRAPHICSEDITOR_DRAWING_MODE_DDA_LINE,
  GRAPHICSEDITOR_DRAWING_MODE_BRESENHAM_LINE,
  GRAPHICSEDITOR_DRAWING_MODE_WU_LINE,
  GRAPHICSEDITOR_DRAWING_MODE_HYPERBOLE,
  GRAPHICSEDITOR_DRAWING_MODE_B_SPLINE
} GraphicsEditorDrawingModeType;

G_END_DECLS

#endif /* GRAPHICSEDITOR_UTILS_H_ */
