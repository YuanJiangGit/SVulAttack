void jswrap_graphics_clear(JsVar *parent) {
  JsGraphics gfx; if (!graphicsGetFromVar(&gfx, parent)) return;
  graphicsClear(&gfx);
  graphicsSetVar(&gfx); // gfx data changed because modified area
}
